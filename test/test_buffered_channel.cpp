#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "chx/Buffered/BufferedChannel.hpp"
#include "doctest.h"
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

using chx::buffered::Channel;

TEST_SUITE("BufferedChannel") {
  TEST_CASE("send and receive concurrently") {
    Channel<int, 100> ch;
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    std::atomic<int> total_sent = 0;
    std::atomic<int> total_received = 0;

    for (int i = 0; i < 10; ++i) {
      producers.emplace_back([&] {
        for (int j = 0; j < 50; ++j) {
          if (ch.try_send(j).has_value()) {
            ++total_sent;
          }
        }
      });
    }

    for (int i = 0; i < 10; ++i) {
      consumers.emplace_back([&] {
        for (int j = 0; j < 50; ++j) {
          auto v = ch.try_receive();
          if (v.has_value()) {
            ++total_received;
          }
        }
      });
    }

    for (auto &t : producers)
      t.join();
    for (auto &t : consumers)
      t.join();

    CHECK(total_sent > 0);
    CHECK(total_received <= total_sent);
  }

  TEST_CASE("Works correctly with concurrent senders") {
    Channel<int, 100> ch;
    std::atomic<int> sends = 0;
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i) {
      threads.emplace_back([&] {
        for (int j = 0; j < 20; ++j) {
          if (ch.try_send(j).has_value()) {
            ++sends;
          }
        }
      });
    }

    for (auto &t : threads)
      t.join();

    CHECK(sends > 0);
    CHECK(sends <= 100);
  }

  TEST_CASE("Works correctly with senders and receivers") {
    Channel<int, 50> ch;
    std::atomic<int> total_sent = 0;
    std::atomic<int> total_received = 0;

    std::thread producer([&] {
      for (int i = 0; i < 100; ++i) {
        if (ch.try_send(i).has_value()) {
          ++total_sent;
        }
      }
    });

    std::thread consumer([&] {
      int attempts = 0;
      while (attempts < 150) {
        auto v = ch.try_receive();
        if (v.has_value()) {
          ++total_received;
        }
        ++attempts;
      }
    });

    producer.join();
    consumer.join();

    CHECK(total_received <= total_sent);
    CHECK(total_sent > 0);
  }

  TEST_CASE("try_send fails when buffer is full") {
    Channel<int, 2> ch;
    CHECK(ch.send(1).has_value());
    CHECK(ch.send(2).has_value());
    auto res = ch.try_send(3);
    CHECK_FALSE(res.has_value());
    CHECK(res.error() == "cannot send inmediatly since the buffer is full");
  }

  TEST_CASE("try_receive fails when buffer is empty") {
    Channel<int, 1> ch;
    auto res = ch.try_receive();
    CHECK_FALSE(res.has_value());
    CHECK(res.error() == "cannot receive inmediatly since the buffer is empty");
  }

  TEST_CASE("receive waits until an element is available") {
    Channel<int, 1> ch;
    std::thread producer([&]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      ch.send(42);
    });
    auto result = ch.receive();
    CHECK(result.has_value());
    CHECK(result.value() == 42);
    producer.join();
  }

  TEST_CASE("send unblocks after receive when buffer was full") {
    Channel<int, 2> ch;
    CHECK(ch.send(1).has_value());
    CHECK(ch.send(2).has_value());
    std::atomic<bool> sent_third{false};
    std::thread producer([&] {
      auto r = ch.send(3);
      if (r.has_value()) {
        sent_third = true;
      }
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    CHECK_FALSE(sent_third.load());
    auto v = ch.receive();
    CHECK(v.has_value());
    producer.join();
    CHECK(sent_third.load());
  }

  TEST_CASE("close prevents further send and receive operations") {
    Channel<int, 1> ch;
    ch.close();
    auto send_res = ch.send(1);
    CHECK_FALSE(send_res.has_value());
    CHECK(send_res.error() == "channel closed");
    auto recv_res = ch.receive();
    CHECK_FALSE(recv_res.has_value());
    CHECK(recv_res.error() == "channel closed");
  }

  TEST_CASE("multiple senders and receivers") {
    Channel<int, 10> ch;
    std::atomic<int> total_produced = 0;
    std::atomic<int> total_consumed = 0;
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < 5; ++i) {
      producers.emplace_back([&]() {
        for (int j = 0; j < 20; ++j) {
          ch.send(j);
          ++total_produced;
        }
      });
    }

    for (int i = 0; i < 5; ++i) {
      consumers.emplace_back([&]() {
        for (int j = 0; j < 20; ++j) {
          auto res = ch.receive();
          if (res.has_value()) {
            ++total_consumed;
          }
        }
      });
    }

    for (auto &p : producers)
      p.join();
    for (auto &c : consumers)
      c.join();

    CHECK(total_produced == 100);
    CHECK(total_consumed == 100);
  }
}

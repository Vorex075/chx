#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "chx/Unbuffered/UnbufferedChannel.hpp"
#include "doctest.h"
#include <atomic>
#include <latch>
#include <thread>
#include <vector>

using chx::unbuffered::Channel;

TEST_SUITE("UnbufferedChannel") {
  TEST_CASE("blocking send/receive transfers value") {
    Channel<int> ch;
    std::atomic<int> received{0};

    std::thread consumer([&] {
      auto v = ch.receive();
      REQUIRE(v.has_value());
      received = *v;
    });

    std::thread producer([&] { ch.send(123); });

    producer.join();
    consumer.join();
    CHECK(received == 123);
  }

  TEST_CASE("try_send fails when no receiver") {
    Channel<int> ch;
    auto res = ch.try_send(5);
    CHECK_FALSE(res.has_value());
  }

  TEST_CASE("try_receive fails when no sender") {
    Channel<int> ch;
    auto res = ch.try_receive();
    CHECK_FALSE(res.has_value());
  }

  TEST_CASE("try_send/try_receive succeed when counterpart waiting") {
    Channel<int> ch;
    std::atomic<bool> ok{false};

    std::latch recv_ready{1};
    std::thread receiver_block([&] {
      recv_ready.count_down();
      auto v = ch.receive();
      REQUIRE(v.has_value());
      ok = (v.value() == 123);
    });

    recv_ready.wait();

    std::thread sender([&] {
      auto error = ch.try_send(123);
      REQUIRE(error.has_value());
    });

    receiver_block.join();
    sender.join();
    CHECK(ok.load());

    ok = false;

    std::latch send_ready{1};

    std::thread sender_block([&] {
      send_ready.count_down();
      auto v = ch.send(42);
    });

    send_ready.wait();

    std::thread receiver([&] {
      auto v = ch.try_receive();
      REQUIRE(v.has_value());
      ok = (*v == 42);
    });

    sender_block.join();
    receiver.join();
    CHECK(ok.load());
  }

  TEST_CASE("many senders and receivers rendez‑vous successfully") {
    constexpr int N = 150;
    Channel<int> ch;
    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};

    std::vector<std::thread> senders;
    std::vector<std::thread> receivers;
    for (int i = 0; i < N / 2; ++i) {
      senders.emplace_back([&] {
        ch.send(i);
        ++produced;
      });
      receivers.emplace_back([&] {
        auto v = ch.receive();
        if (v.has_value()) {
          ++consumed;
        }
      });
    }

    for (int i{0}; i < N / 2; ++i) {
      receivers.emplace_back([&] {
        auto v = ch.receive();
        if (v.has_value()) {
          ++consumed;
        }
      });
      senders.emplace_back([&] {
        ch.send(i);
        ++produced;
      });
    }

    for (auto &t : senders)
      t.join();
    for (auto &t : receivers)
      t.join();

    CHECK(produced == N);
    CHECK(consumed == N);
  }
}

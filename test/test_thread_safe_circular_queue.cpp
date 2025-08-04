#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../include/circular_queue/circular_queue_thread_safe.hpp"
#include "doctest.h"
#include <thread>
#include <vector>

TEST_SUITE("ThreadSafeCircularQueue") {
  TEST_CASE("Push and pop concurrently") {
	std::cout << "test1";
    chx::ThreadSafeCircularQueue<int, 100> q;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    std::atomic<int> total_pushed = 0;
    std::atomic<int> total_popped = 0;

    // 10 producers
    for (int i{0}; i < 10; ++i) {
      producers.emplace_back([&] {
        for (int j{0}; j < 50; ++j) {
          if (q.push(j)) {
            ++total_pushed;
          }
        }
      });
    }

    // 10 consumers
    for (int i = 0; i < 10; ++i) {
      consumers.emplace_back([&] {
        for (int j{0}; j < 50; ++j) {
          if (q.front()) {
            q.pop();
            ++total_popped;
          }
        }
      });
    }

    for (auto &t : producers)
      t.join();
    for (auto &t : consumers)
      t.join();

    CHECK(total_pushed > 0);
    CHECK(total_popped <= total_pushed);
  }

  TEST_CASE("Works correctly with concurrent producers") {
	std::cout << "test2";
    chx::ThreadSafeCircularQueue<int, 100> q;
    std::atomic<int> pushes = 0;

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
      threads.emplace_back([&] {
        for (int j = 0; j < 20; ++j) {
          if (q.push(j)) {
            ++pushes;
          };
        }
      });
    }

    for (auto &t : threads) {
      t.join();
    }
    CHECK(pushes > 0);
    CHECK(pushes <= 100);
  }

  TEST_CASE("Works correctly with producers and consumers") {
	std::cout << "test3";
    chx::ThreadSafeCircularQueue<int, 50> q;
    std::atomic<int> total_pushed = 0;
    std::atomic<int> total_popped = 0;

    std::thread producer([&] {
      for (int i = 0; i < 100; ++i) {
        if (q.push(i)) {
          ++total_pushed;
        }
      }
    });

    std::thread consumer([&] {
      int attempts = 0;
      while (attempts < 150) {
        if (q.front()) {
          q.pop();
          ++total_popped;
        }
        ++attempts;
      }
    });

    producer.join();
    consumer.join();

    CHECK(total_popped <= total_pushed);
    CHECK(total_pushed > 0);
  }
  TEST_CASE("push() notifies not_empty_ only if queue was empty") {
	std::cout << "test4";
    chx::ThreadSafeCircularQueue<int, 2> q;
    CHECK(q.push(1).value());
    CHECK(q.push(2).value());
    auto result = q.push(3);
    CHECK_FALSE(result.value());
  }

  TEST_CASE("push_timeout fails if queue is full") {
	std::cout << "test5";
    chx::ThreadSafeCircularQueue<int, 1> q;
    q.push(1);
    auto result = q.push_timeout(2, std::chrono::milliseconds(1));
    CHECK_FALSE(result.has_value());
    CHECK(result.error() == "The wait for pushing timed out");
  }

  TEST_CASE("pop_front_blocking waits until an element is available") {
	std::cout << "test5";
    chx::ThreadSafeCircularQueue<int, 1> q;
    std::thread producer([&]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      q.push(42);
    });
    auto result = q.pop_front_blocking();
    CHECK(result.has_value());
    CHECK(result.value() == 42);
    producer.join();
  }

  TEST_CASE("pop_front_timeout fails if no element arrives in time") {
	std::cout << "test6";
    chx::ThreadSafeCircularQueue<int, 1> q;
    auto result = q.pop_front_timeout(std::chrono::milliseconds(1));
    CHECK_FALSE(result.has_value());
    CHECK(result.error() == "The wait for popping timed out");
  }

  TEST_CASE("pop_front notifies not_full_ if queue was full") {
	std::cout << "test7";
    chx::ThreadSafeCircularQueue<int, 2> q;
    q.push(1);
    q.push(2);
    auto result = q.pop_front();
    CHECK(result.has_value());
    CHECK((result.value() == 1 || result.value() == 2));
    CHECK(q.push(3).value());
  }

  TEST_CASE("multiple producers and consumers") {
	std::cout << "test8";
    chx::ThreadSafeCircularQueue<int, 10> q;
    std::atomic<int> total_produced = 0;
    std::atomic<int> total_consumed = 0;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < 5; ++i) {
      producers.emplace_back([&]() {
        for (int j = 0; j < 20; ++j) {
          while (!q.push(j).value()) {
            std::this_thread::yield();
          }
          total_produced++;
        }
      });
    }

    for (int i = 0; i < 5; ++i) {
      consumers.emplace_back([&]() {
        for (int j = 0; j < 20; ++j) {
          auto res = q.pop_front_blocking();
          if (res.has_value()) {
            total_consumed++;
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

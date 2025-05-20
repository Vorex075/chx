#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../include/circular_queue/circular_queue_thread_safe.hpp"
#include "doctest.h"
#include <thread>
#include <vector>

TEST_SUITE("ThreadSafeCircularQueue") {
  TEST_CASE("Push and pop concurrently") {
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
}

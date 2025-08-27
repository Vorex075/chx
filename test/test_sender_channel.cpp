#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "chx/SenderChannel.hpp"
#include "chx/Unbuffered/UnbufferedChannel.hpp"
#include "chx/channel.hpp"
#include "chx/channel_factory.hpp"
#include "doctest.h"
#include <atomic>
#include <thread>

TEST_SUITE("SenderChannel") {
  TEST_CASE("send delivers value to receiver") {
    chx::Channel<int> ch = chx::CreateChannel<int>();
    auto sender = ch.make_sender();
    std::atomic<int> received{0};

    std::thread consumer([&] {
      auto v = ch.receive();
      REQUIRE(v.has_value());
      received = *v;
    });

    std::thread producer([&] {
      auto res = sender.send(123);
      REQUIRE(res.has_value());
    });

    producer.join();
    consumer.join();
    CHECK(received == 123);
  }

  TEST_CASE("try_send fails when no receiver waiting") {
    chx::Channel<int> ch = chx::CreateChannel<int>();
    auto sender = ch.make_sender();
    auto res = sender.try_send(5);
    CHECK_FALSE(res.has_value());
  }

  TEST_CASE("close prevents further send operations") {
    chx::Channel<int> ch = chx::CreateChannel<int>();
    auto sender = ch.make_sender();
    sender.close();
    auto res = sender.send(1);
    CHECK_FALSE(res.has_value());
    CHECK(res.error() == std::string("channel closed"));
  }
}

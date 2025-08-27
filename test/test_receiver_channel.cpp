#include "chx/channel_factory.hpp"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "chx/ReceiverChannel.hpp"
#include "chx/channel.hpp"
#include "chx/channel_factory.hpp"
#include "doctest.h"
#include <atomic>
#include <thread>

TEST_SUITE("ReceiverChannel") {
  TEST_CASE("receive obtains value from sender") {
    chx::Channel<int> ch = chx::CreateChannel<int>();
    auto receiver = ch.make_receiver();
    std::atomic<int> received{0};

    std::thread producer([&] {
      auto res = ch.send(77);
      REQUIRE(res.has_value());
    });

    auto v = receiver.receive();
    REQUIRE(v.has_value());
    received = *v;
    producer.join();
    CHECK(received == 77);
  }

  TEST_CASE("try_receive fails when no value available") {
    chx::Channel<int> ch = chx::CreateChannel<int>();
    auto receiver = ch.make_receiver();
    auto res = receiver.try_receive();
    CHECK_FALSE(res.has_value());
  }

  TEST_CASE("close prevents further receive operations") {
    chx::Channel<int> ch = chx::CreateChannel<int>();
    auto receiver = ch.make_receiver();
    receiver.close();
    auto res = receiver.receive();
    CHECK_FALSE(res.has_value());
    CHECK(res.error() == std::string("channel closed"));
  }
}

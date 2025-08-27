#pragma once

#include "chx/channel.hpp"
#include "chx/channelCore.hpp"
#include <memory>

namespace chx {
template <typename T> class SenderChannel {
public:
  SenderChannel() = delete;
  ~SenderChannel() = default;

  std::expected<void, Error> send(T &&value) {
    return core_->send(std::move(value));
  }
  std::expected<void, Error> send(const T &value) { return core_->send(value); }

  std::expected<void, Error> try_send(T &&value) {
    return core_->try_send(std::move(value));
  }
  std::expected<void, Error> try_send(const T &value) {
    return core_->try_send(value);
  }

  void close() { this->core_->close(); }
  bool is_closed() { return this->core_->is_closed(); }

  friend Channel<T>;

private:
  explicit SenderChannel(std::shared_ptr<ChannelCore<T>> core) : core_(core) {}
  std::shared_ptr<chx::ChannelCore<T>> core_;
};

} // namespace chx

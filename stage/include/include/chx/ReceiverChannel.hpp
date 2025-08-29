#pragma once

#include "channel.hpp"

namespace chx {

template <typename T> class ReceiverChannel {
public:
  ReceiverChannel() = delete;
  ~ReceiverChannel() = default;

  std::expected<T, Error> receive() { return this->core_->receive(); }
  std::expected<T, Error> try_receive() { return this->core_->try_receive(); }

  void close() { this->core_->close(); }
  bool is_closed() { return this->core_->is_closed(); }

  friend Channel<T>;

private:
  explicit ReceiverChannel(std::shared_ptr<ChannelCore<T>> core)
      : core_(core) {}
  std::shared_ptr<ChannelCore<T>> core_;
};

} // namespace chx

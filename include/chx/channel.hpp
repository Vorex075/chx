#pragma once

#include "channelCore.hpp"
#include <memory>

namespace chx {

template <typename T> class Channel {
public:
  Channel(std::shared_ptr<ChannelCore<T>> core) : core_(core) {}
  ~Channel() = default;

  /**
   *  @brief Sends an object through the channel. This method blocks the thread
   * until the operation is done.
   *  @param value The object to be sent through the channel.
   *  @returns `void` if the operation was successful. An `Error` if the
   * operation failed.
   * */
  std::expected<void, Error> send(T &&value) {
    return core_->send(std::move(value));
  }
  std::expected<void, Error> send(const T &value) { return core_->send(value); }

  /**
   *  @brief Sends an object through the channel. This method does not block
   * the thread, so an error will be returned if the operation could not be done
   * inmediately.
   *  @param value The object to be sent through the channel.
   *  @returns `void` if the operation wasa successful. An `Error` if the
   * operation failed.
   * */
  std::expected<void, Error> try_send(T &&value) {
    return core_->try_send(std::move(value));
  }
  std::expected<void, Error> try_send(const T &value) {
    return core_->try_send(value);
  }

  /**
   *  @brief Receives an object through the channel. This method blocks the
   * thread until the operation is done.
   *  @returns An object (the one received from the channel), or an `Error` if
   * the operation failed.
   * */
  std::expected<T, Error> receive() { return core_->receive(); };

  /**
   *  @brief Receives an object through the channel. This method does not block
   *  the thread, so an error will be returned if the operation could not be
   * done inmediately.
   *  @param value The object to be sent through the channel.
   *  @returns An object (the one received from the channel), or an `Error` if
   * the operation failed.
   * */
  std::expected<T, Error> try_receive() { return core_->try_receive(); };

  virtual void close();
  virtual bool is_closed();

private:
  std::shared_ptr<ChannelCore<T>> core_;
};

} // namespace chx

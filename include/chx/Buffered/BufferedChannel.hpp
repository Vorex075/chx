#pragma once

#include "chx/Buffered/circular_queue.hpp"
#include "chx/channelCore.hpp"
#include <condition_variable>
#include <mutex>
#include <utility>

namespace chx::buffered {

template <typename T, std::size_t Capacity>
class Channel : public chx::ChannelCore<T> {
public:
  Channel() : chx::ChannelCore<T>(), closed(false) {}
  ~Channel() = default;

  std::expected<void, Error> send(const T &value) override;
  std::expected<void, Error> send(T &&value) override;
  std::expected<void, Error> try_send(T &&value) override;
  std::expected<void, Error> try_send(const T &value) override;

  std::expected<T, Error> receive() override;
  std::expected<T, Error> try_receive() override;

  virtual void close() override;
  virtual bool is_closed() const override;

  Channel<T, Capacity> &operator=(const Channel<T, Capacity> &ch) = delete;

private:
  template <typename U>
    requires std::constructible_from<T, U &&>
  std::expected<void, Error> send_(U &&value);

  template <typename U>
    requires std::constructible_from<T, U &&>
  std::expected<void, Error> try_send_(U &&value);

  mutable std::mutex mutex;
  std::condition_variable not_empty;
  std::condition_variable not_full;
  CircularQueue<T, Capacity> queue;
  bool closed = false;
};

template <typename T, std::size_t Capacity> void Channel<T, Capacity>::close() {
  std::lock_guard lock(this->mutex);
  this->closed = true;
  this->receiver_entrance.notify_all();
  this->sender_entrance.notify_all();
  return;
}

template <typename T, std::size_t Capacity>
bool Channel<T, Capacity>::is_closed() const {
  std::lock_guard lock(this->mutex);
  return this->closed == true;
}

template <typename T, std::size_t Capacity>
std::expected<void, Error> Channel<T, Capacity>::send(const T &value) {
  return this->send_(value);
}

template <typename T, std::size_t Capacity>
std::expected<void, Error> Channel<T, Capacity>::send(T &&value) {
  return this->send_(value);
}

template <typename T, std::size_t Capacity>
std::expected<void, Error> Channel<T, Capacity>::try_send(const T &value) {
  return this->try_send_(value);
}

template <typename T, std::size_t Capacity>
std::expected<void, Error> Channel<T, Capacity>::try_send(T &&value) {
  return this->try_send_(value);
}

template <typename T, std::size_t Capacity>
template <typename U>
  requires std::constructible_from<T, U &&>
std::expected<void, Error> Channel<T, Capacity>::send_(U &&value) {
  std::unique_lock lock(this->mutex);
  this->not_full.wait(lock,
                      [&] { return !this->queue.is_full() || this->closed; });
  if (this->closed) {
    return std::unexpected("channel closed");
  }
  this->queue.push(std::forward<T>(value));
  this->not_empty.notify_one();
  return {};
}

template <typename T, std::size_t Capacity>
template <typename U>
  requires std::constructible_from<T, U &&>
std::expected<void, Error> Channel<T, Capacity>::try_send_(U &&value) {
  std::unique_lock lock(this->mutex);
  if (this->closed) {
    return std::unexpected("channel closed");
  }
  if (this->queue.is_full()) {
    return std::unexpected("cannot send inmediatly since the buffer is full");
  }
  this->queue.push(std::forward<T>(value));
  this->not_empty.notify_one();
  return {};
}

template <typename T, std::size_t Capacity>
std::expected<T, Error> Channel<T, Capacity>::receive() {
  std::unique_lock lock(this->mutex);
  this->not_empty.wait(lock,
                       [&] { return !this->queue.is_empty() || this->closed; });
  if (this->closed) {
    return std::unexpected("channel closed");
  }
  auto value = std::move(*this->queue.front());
  this->queue.pop();
  this->not_full.notify_one();
  return value;
}

template <typename T, std::size_t Capacity>
std::expected<T, Error> Channel<T, Capacity>::try_receive() {
  std::unique_lock lock(this->mutex);
  if (this->closed) {
    return std::unexpected("channel closed");
  }
  if (this->queue.is_empty()) {
    return std::unexpected(
        "cannot receive inmediatly since the buffer is empty");
  }
  auto value = std::move(*this->queue.front());
  this->queue.pop();
  this->not_empty.notify_one();
  return value;
}
} // namespace chx::buffered

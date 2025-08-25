#pragma once

#include "../channel.hpp"
#include "BufferedDataHandler.hpp"
#include <utility>

namespace chx::buffered {

template <typename T, std::size_t Capacity>
class Channel : public chx::Channel<T> {
public:
  Channel() : chx::Channel<T>(), data_() {}
  Channel(const Channel<T, Capacity> &ch) = delete;
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
  DataHandler<T, Capacity> data_;
  template <typename U>
    requires std::constructible_from<T, U &&>
  std::expected<void, Error> send_(U &&value);

  template <typename U>
    requires std::constructible_from<T, U &&>
  std::expected<void, Error> try_send_(U &&value);
};

template <typename T, std::size_t Capacity> void Channel<T, Capacity>::close() {
  std::lock_guard lock(this->data_.mutex);
  this->data_.closed = true;
  this->data_.not_empty.notify_all();
  this->data_.not_full.notify_all();
  return;
}

template <typename T, std::size_t Capacity>
bool Channel<T, Capacity>::is_closed() const {
  std::lock_guard lock(this->data_.mutex);
  return this->data_.closed == true;
}

template <typename T, std::size_t Capacity>
std::expected<void, Error> Channel<T, Capacity>::send(const T &value) {
  return this->send_(value);
}

template <typename T, std::size_t Capacity>
std::expected<void, Error> Channel<T, Capacity>::send(T &&value) {
  return this->send_(std::move(value));
}

template <typename T, std::size_t Capacity>
std::expected<void, Error> Channel<T, Capacity>::try_send(const T &value) {
  return this->try_send_(value);
}

template <typename T, std::size_t Capacity>
std::expected<void, Error> Channel<T, Capacity>::try_send(T &&value) {
  return this->try_send_(std::move(value));
}

template <typename T, std::size_t Capacity>
template <typename U>
  requires std::constructible_from<T, U &&>
std::expected<void, Error> Channel<T, Capacity>::send_(U &&value) {
  std::unique_lock lock(this->data_.mutex);
  this->data_.not_full.wait(
      lock, [&] { return !this->data_.queue.is_full() || this->data_.closed; });
  if (this->data_.closed) {
    return std::unexpected("channel closed");
  }
  this->data_.queue.push(std::forward<U>(value));
  this->data_.not_empty.notify_one();
  return {};
}

template <typename T, std::size_t Capacity>
template <typename U>
  requires std::constructible_from<T, U &&>
std::expected<void, Error> Channel<T, Capacity>::try_send_(U &&value) {
  std::unique_lock lock(this->data_.mutex);
  if (this->data_.closed) {
    return std::unexpected("channel closed");
  }
  if (this->data_.queue.is_full()) {
    return std::unexpected("cannot send inmediatly since the buffer is full");
  }
  this->data_.queue.push(std::forward<U>(value));
  this->data_.not_empty.notify_one();
  return {};
}

template <typename T, std::size_t Capacity>
std::expected<T, Error> Channel<T, Capacity>::receive() {
  std::unique_lock lock(this->data_.mutex);
  this->data_.not_empty.wait(lock, [&] {
    return !this->data_.queue.is_empty() || this->data_.closed;
  });
  if (this->data_.closed) {
    return std::unexpected("channel closed");
  }
  auto value = std::move(*this->data_.queue.front());
  this->data_.queue.pop();
  this->data_.not_full.notify_one();
  return value;
}

template <typename T, std::size_t Capacity>
std::expected<T, Error> Channel<T, Capacity>::try_receive() {
  std::unique_lock lock(this->data_.mutex);
  if (this->data_.closed) {
    return std::unexpected("channel closed");
  }
  if (this->data_.queue.is_empty()) {
    return std::unexpected(
        "cannot receive inmediatly since the buffer is empty");
  }
  auto value = std::move(*this->data_.queue.front());
  this->data_.queue.pop();
  this->data_.not_full.notify_one();
  return value;
}
} // namespace chx::buffered

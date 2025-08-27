#pragma once

#include "chx/channelCore.hpp"
#include <condition_variable>
#include <mutex>
#include <optional>
#include <utility>

namespace chx::unbuffered {
template <typename T> class Channel : public chx::ChannelCore<T> {
public:
  Channel()
      : chx::ChannelCore<T>(), value_set(false), closed(false),
        receivers_waiting(0) {}
  ~Channel() = default;

  std::expected<void, Error> send(const T &value) override;
  std::expected<void, Error> send(T &&value) override;
  std::expected<void, Error> try_send(T &&value) override;
  std::expected<void, Error> try_send(const T &value) override;

  std::expected<T, Error> receive() override;
  std::expected<T, Error> try_receive() override;

  virtual void close() override;
  virtual bool is_closed() const override;

  Channel<T> &operator=(const Channel<T> &ch) = delete;

private:
  template <typename U>
    requires std::constructible_from<T, U &&>
  std::expected<void, Error> send_(U &&value);

  template <typename U>
    requires std::constructible_from<T, U &&>
  std::expected<void, Error> try_send_(U &&value);

  mutable std::mutex mutex;
  std::condition_variable sender_entrance;
  std::condition_variable sender_exit;
  std::condition_variable receiver_entrance;
  bool value_set = false;
  std::optional<T> slot;
  bool closed = false;
  unsigned int receivers_waiting = 0;
};

template <typename T> void Channel<T>::close() {
  std::lock_guard lock(this->mutex);
  this->closed = true;
  this->receiver_entrance.notify_all();
  this->sender_entrance.notify_all();
  return;
}

template <typename T> bool Channel<T>::is_closed() const {
  std::lock_guard lock(this->mutex);
  return this->closed == true;
}

template <typename T>
std::expected<void, Error> Channel<T>::send(const T &value) {
  return this->send_(value);
}

template <typename T> std::expected<void, Error> Channel<T>::send(T &&value) {
  return this->send_(value);
}

template <typename T>
std::expected<void, Error> Channel<T>::try_send(const T &value) {
  return this->try_send_(value);
}

template <typename T>
std::expected<void, Error> Channel<T>::try_send(T &&value) {
  return this->try_send_(value);
}

template <typename T>
template <typename U>
  requires std::constructible_from<T, U &&>
std::expected<void, Error> Channel<T>::send_(U &&value) {
  std::unique_lock lock(this->mutex);
  this->sender_entrance.wait(lock,
                             [&] { return !this->value_set || this->closed; });
  if (this->closed) {
    return std::unexpected("channel closed");
  }
  this->slot.emplace(std::forward<U>(value));
  this->value_set = true;
  this->receiver_entrance.notify_one();

  this->sender_exit.wait(lock);
  this->sender_entrance.notify_one();

  return {};
}

template <typename T>
template <typename U>
  requires std::constructible_from<T, U &&>
std::expected<void, Error> Channel<T>::try_send_(U &&value) {
  std::unique_lock lock(this->mutex);
  if (this->value_set) {
    return std::unexpected("cannot send instantly");
  }
  if (this->closed) {
    return std::unexpected("channel closed");
  }
  if (this->receivers_waiting == 0) {
    return std::unexpected("cannot send instantly");
  }
  this->slot.emplace(std::forward<U>(value));
  this->value_set = true;
  this->receiver_entrance.notify_one();

  this->sender_exit.wait(lock);
  this->sender_entrance.notify_one();
  return {};
}

template <typename T> std::expected<T, Error> Channel<T>::receive() {
  std::unique_lock lk(this->mutex);
  this->receivers_waiting++;
  this->receiver_entrance.wait(lk,
                               [&] { return this->value_set || this->closed; });
  this->receivers_waiting--;
  if (this->closed && !this->value_set) {
    return std::unexpected("channel closed");
  }

  T value_read = std::move(*this->slot);
  this->slot.reset();
  this->value_set = false;
  this->sender_exit.notify_one();
  return value_read;
}

template <typename T> std::expected<T, Error> Channel<T>::try_receive() {
  std::unique_lock lock(this->mutex);
  if (this->closed) {
    return std::unexpected("channel closed");
  }
  if (!this->value_set) {
    return std::unexpected("cannot receive instantly");
  }

  T value_read = std::move(this->slot.value());
  this->slot.reset();
  this->value_set = false;
  this->sender_exit.notify_one();
  return value_read;
}
} // namespace chx::unbuffered

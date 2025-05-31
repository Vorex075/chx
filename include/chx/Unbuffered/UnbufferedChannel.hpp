#pragma once

#include "UnbufferedDataHandler.hpp"
#include "chx/channel.hpp"
#include <mutex>
#include <utility>

namespace chx::unbuffered {
template <typename T> class Channel : public chx::Channel<T> {
public:
  Channel() : chx::Channel<T>(), data_() {}
  Channel(const Channel<T> &ch) = delete;
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

  DataHandler<T> data_;
};

template <typename T> void Channel<T>::close() {
  std::lock_guard lock(this->data_.mutex);
  this->data_.closed = true;
  this->data_.receiver_entrance.notify_all();
  this->data_.sender_entrance.notify_all();
  return;
}

template <typename T> bool Channel<T>::is_closed() const {
  std::lock_guard lock(this->data_.mutex);
  return this->data_.closed == true;
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
  std::unique_lock lock(this->data_.mutex);
  this->data_.sender_entrance.wait(
      lock, [&] { return !this->data_.value_set || this->data_.closed; });
  if (this->data_.closed) {
    return std::unexpected("channel closed");
  }
  this->data_.slot.emplace(std::forward<U>(value));
  this->data_.value_set = true;
  this->data_.receiver_entrance.notify_one();

  this->data_.sender_exit.wait(lock);
  this->data_.sender_entrance.notify_one();

  return {};
}

template <typename T>
template <typename U>
  requires std::constructible_from<T, U &&>
std::expected<void, Error> Channel<T>::try_send_(U &&value) {
  std::unique_lock lock(this->data_.mutex);
  if (this->data_.value_set) {
    return std::unexpected("cannot send instantly");
  }
  if (this->data_.closed) {
    return std::unexpected("channel closed");
  }
  if (this->data_.receivers_waiting == 0) {
    return std::unexpected("cannot send instantly");
  }
  this->data_.slot.emplace(std::forward<U>(value));
  this->data_.value_set = true;
  this->data_.receiver_entrance.notify_one();

  this->data_.sender_exit.wait(lock);
  this->data_.sender_entrance.notify_one();
  return {};
}

template <typename T> std::expected<T, Error> Channel<T>::receive() {
  std::unique_lock lk(this->data_.mutex);
  this->data_.receivers_waiting++;
  this->data_.receiver_entrance.wait(
      lk, [&] { return this->data_.value_set || this->data_.closed; });
  this->data_.receivers_waiting--;
  if (this->data_.closed && !this->data_.value_set) {
    return std::unexpected("channel closed");
  }

  T value_read = std::move(*this->data_.slot);
  this->data_.slot.reset();
  this->data_.value_set = false;
  this->data_.sender_exit.notify_one();
  return value_read;
}

template <typename T> std::expected<T, Error> Channel<T>::try_receive() {
  std::unique_lock lock(this->data_.mutex);
  if (this->data_.closed) {
    return std::unexpected("channel closed");
  }
  if (!this->data_.value_set) {
    return std::unexpected("cannot receive instantly");
  }

  T value_read = std::move(this->data_.slot.value());
  this->data_.slot.reset();
  this->data_.value_set = false;
  this->data_.sender_exit.notify_one();
  return value_read;
}
} // namespace chx::unbuffered

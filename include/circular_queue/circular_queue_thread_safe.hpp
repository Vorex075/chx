#pragma once

#include "circular_queue.hpp"
#include <chrono>
#include <condition_variable>
#include <expected>
#include <mutex>

namespace chx {
template <typename T, std::size_t Capacity> class ThreadSafeCircularQueue {
public:
  ThreadSafeCircularQueue() = default;

  /**
   *  @brief Pushes a new element into the queue, ensuring thread safety.
   *  @param value The new value to be inserted into the queue. It will be
   *  `moved` on insertion.
   *  @returns True if the value was successfully inserted into the queue.
   *  False otherwise. An error message is never returned in this method.
   * */
  std::expected<bool, std::string> push(T &&value);

  /**
   *  @brief Pushes a new element into the queue, ensuring thread safety. This
   * method blocks the thread until there is space in the queue to push a new
   * element.
   *  @param value The new value to be inserted into the queue. It will be
   * `moved` on insertion.
   *  @returns True if the value was successfully inserted into the queue. An
   * unexpected std::string error message is returned if the queue is closed.
   * */
  std::expected<bool, std::string> push_blocking(T &&value);

  /**
   *  @brief Pushes a new element into the queue, ensuring thread safety. This
   * method blocks the thread until there is space in the queue to push a new
   * element or the specified timeout run out.
   * @param value The new value to be inserted into the queue. It will be
   * `moved` on insertion.
   * @param timeout The wait timeout, in milliseconds.
   * @returns True if the value was successfully inserted into the queue. An
   * unexpected std::string error message is returned if the queue is closed or
   * the timeout run out.
   * */
  std::expected<bool, std::string> push_timeout(
      T &&value,
      std::chrono::milliseconds timeout = std::chrono::milliseconds(0));

  /**
   *  @brief Pushes a new element into the queue.
   *  @param value The new value to be inserted into the queue. It will be
   *  copied on insertion.
   *  @returns True if the value was successfully inserted into the queue.
   *  False otherwise. An error message is never returned in this method.
   * */
  std::expected<bool, std::string> push(const T &value);

  std::expected<bool, std::string> push_blocking(const T &value);

  std::expected<bool, std::string> push_timeout(
      const T &value,
      std::chrono::milliseconds timeout = std::chrono::milliseconds(0));

  /**
   *  @brief Gets the element at the front of the queue, ensuring thread safety.
   *  @returns A pointer to the element at the front of the queue. If the queue
   *  is empty, nullprt will be returned.
   * */
  T *front();

  /**
   *  @brief Deletes the element at the front of the queue. If there is no
   * element, this function does nothing. This function ensures thread safety.
   * */
  void pop();

  /**
   *  @brief Deletes the element at the front of the queue and returns it. If
   * there is no element, this function returns an string with the error. This
   * function ensures thread safety.
   * */
  std::expected<T, std::string> pop_front();

  /**
   *  @brief Deletes the element at the front of the queue and returns it. This
   * function ensures thread safety and blocks the thread until there is an
   * element to be deleted from the queue.
   *  @returns The value at the front of the queue, or an unexpected std::string
   * if the queue is closed.
   * */
  std::expected<T, std::string> pop_front_blocking();

  /**
   *  @brief Deletes the element at the front of the queue and returns it. This
   * method ensures thread safety and blocks the thread for an specified time
   * (in milliseconds) or until there is an element to be deleted from the
   * queue.
   *  @param timeout The maximun time to wait for an element to be deleted.
   *  @returns The valur at the front of the queue, or an unexpected std::string
   * if the timeut runs out or the queue is closed.
   *
   * */
  std::expected<T, std::string>
  pop_front_timeout(std::chrono::milliseconds timeout);

  /**
   *  @returns The maximun size of the queue.
   * */
  std::size_t max_size() const { return Capacity; }

  /**
   *  @return The number of elements left in the queue.
   * */
  std::size_t size() const { return this->queue_.size(); }

private:
  class CircularQueue<T, Capacity> queue_;
  mutable std::mutex mutex_;
  std::condition_variable not_empty_;
  std::condition_variable not_full_;
};

template <typename T, std::size_t Capacity>
std::expected<bool, std::string>
ThreadSafeCircularQueue<T, Capacity>::push(T &&value) {
  std::unique_lock lock(this->mutex_);
  if (this->queue_.is_empty()) {
    this->not_empty_.notify_one();
  }
  return this->queue_.push(std::move(value));
}

template <typename T, std::size_t Capacity>
std::expected<bool, std::string>
ThreadSafeCircularQueue<T, Capacity>::push_blocking(T &&value) {
  std::unique_lock lock(this->mutex_);
  this->not_full_.wait(lock, [&] { return !this->queue_.is_full(); });
  if (this->queue_.is_empty()) {
    this->not_empty_.notify_one();
  }
  return this->queue_.push(std::move(value));
}

template <typename T, std::size_t Capacity>
std::expected<bool, std::string>
ThreadSafeCircularQueue<T, Capacity>::push_timeout(
    T &&value, std::chrono::milliseconds timeout) {
  std::unique_lock lock(this->mutex_);
  if (!this->not_full_.wait_for(lock, timeout,
                                [&] { return !this->queue_.is_full(); })) {
    return std::unexpected("The wait for pushing timed out");
  }
  if (this->queue_.is_empty()) {
    this->not_empty_.notify_one();
  }
  return this->queue_.push(std::move(value));
}

template <typename T, std::size_t Capacity>
std::expected<bool, std::string>
ThreadSafeCircularQueue<T, Capacity>::push(const T &value) {
  std::unique_lock lock(this->mutex_);
  if (this->queue_.is_empty()) {
    this->not_empty_.notify_one();
  }
  return this->queue_.push(value);
}

template <typename T, std::size_t Capacity>
std::expected<bool, std::string>
ThreadSafeCircularQueue<T, Capacity>::push_blocking(const T &value) {
  std::unique_lock lock(this->mutex_);
  this->not_full_.wait(lock, [&] { return !this->queue_.is_full(); });
  if (this->queue_.is_empty()) {
    this->not_empty_.notify_one();
  }
  return this->queue_.push(value);
}

template <typename T, std::size_t Capacity>
std::expected<bool, std::string>
ThreadSafeCircularQueue<T, Capacity>::push_timeout(
    const T &value, std::chrono::milliseconds timeout) {
  std::unique_lock lock(this->mutex_);
  if (!this->not_full_.wait_for(lock, timeout,
                                [&] { return !this->queue_.is_full(); })) {
    return std::unexpected("The wait for pushing timed out");
  }
  if (this->queue_.is_empty()) {
    this->not_empty_.notify_one();
  }
  return this->queue_.push(value);
}

template <typename T, std::size_t Capacity>
T *ThreadSafeCircularQueue<T, Capacity>::front() {
  std::unique_lock lock(this->mutex_);
  return this->queue_.front();
}

template <typename T, std::size_t Capacity>
void ThreadSafeCircularQueue<T, Capacity>::pop() {
  std::unique_lock lock(this->mutex_);
  if (this->queue_.is_full()) {
    this->not_full_.notify_one();
  }
  this->queue_.pop();
  return;
}

template <typename T, std::size_t Capacity>
std::expected<T, std::string>
ThreadSafeCircularQueue<T, Capacity>::pop_front() {
  std::unique_lock lock(this->mutex_);
  auto value = this->queue_.front();
  if (value == nullptr) {
    return std::unexpected("The queue is empty");
  }
  if (this->queue_.is_full()) {
    this->not_full_.notify_one();
  }
  T value_to_return{std::move(*value)};
  this->queue_.pop();
  return value_to_return;
}

template <typename T, std::size_t Capacity>
std::expected<T, std::string>
ThreadSafeCircularQueue<T, Capacity>::pop_front_blocking() {
  std::unique_lock lock(this->mutex_);
  this->not_empty_.wait(lock, [&] { return !this->queue_.is_empty(); });
  // The queue is not empty
  if (this->queue_.is_full()) {
    this->not_full_.notify_one();
  }
  T value = std::move(*this->queue_.front());
  this->queue_.pop();
  return value;
}

template <typename T, std::size_t Capacity>
std::expected<T, std::string>
ThreadSafeCircularQueue<T, Capacity>::pop_front_timeout(
    std::chrono::milliseconds timeout) {
  std::unique_lock lock(this->mutex_);
  if (!this->not_empty_.wait_for(lock, timeout,
                                 [&] { return !this->queue_.is_empty(); })) {
    return std::unexpected("The wait for popping timed out");
  }
  // The queue is not empty
  if (this->queue_.is_full()) {
    this->not_full_.notify_one();
  }
  T value = std::move(*this->queue_.front());
  this->queue_.pop();
  return value;
}

} // namespace chx

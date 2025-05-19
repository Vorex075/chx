#pragma once

#include "circular_queue.hpp"
#include <condition_variable>
#include <mutex>

namespace chx {
template <typename T, std::size_t Capacity>
class ThreadSafeCircularQueue {
public:
  ThreadSafeCircularQueue() = default;

  /**
   *  @brief Pushes a new element into the queue, ensuring thread safety.
   *  @param value The new value to be inserted into the queue. It will be
   *  `moved` on insertion.
   *  @returns True if the value was successfully inserted into the queue.
   *  False otherwise.
   * */
  bool push(T&& value);

  /**
   *  @brief Pushes a new element into the queue.
   *  @param value The new value to be inserted into the queue. It will be
   *  copied on insertion.
   *  @returns True if the value was successfully inserted into the queue.
   *  False otherwise.
   * */
  bool push(const T& value);

  /**
   *  @brief Gets the element at the front of the queue, ensuring thread safety.
   *  @returns A pointer to the element at the front of the queue. If the queue
   *  is empty, nullprt will be returned.
   * */
  T* front(); 

  /**
   *  @brief Deletes the element at the front of the queue. If there is no
   *  element, this function does nothing. This function ensures thread safety.
   * */
  void pop();

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
bool ThreadSafeCircularQueue<T, Capacity>::push(T&& value) {
  std::unique_lock lock(this->mutex_); 
  return this->queue_.push(std::move(value));
}

template <typename T, std::size_t Capacity>
bool ThreadSafeCircularQueue<T, Capacity>::push(const T& value) {
  std::unique_lock lock(this->mutex_);
  return this->queue_.push(value);
}

template <typename T, std::size_t Capacity>
T* ThreadSafeCircularQueue<T, Capacity>::front() {
  std::unique_lock lock(this->mutex_);
  return this->queue_.front();
}

template <typename T, std::size_t Capacity>
void ThreadSafeCircularQueue<T, Capacity>::pop() {
  std::unique_lock lock(this->mutex_);
  this->queue_.pop();
  return;
}

}

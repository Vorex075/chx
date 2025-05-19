#pragma once

#include <array>

namespace chx {
template <typename T, std::size_t Capacity>
requires (Capacity > 0)
class CircularQueue {
public:
  CircularQueue() = default;

  CircularQueue(const CircularQueue &) = delete;
  CircularQueue(CircularQueue &&) = delete;
  CircularQueue &operator=(const CircularQueue &) = delete;
  CircularQueue &operator=(CircularQueue &&) = delete;

  /**
   *  @brief Pushes a new element into the queue.
   *  @param value The new value to be inserted into the queue. It will be
   *  copied on insertion.
   *  @returns True if the value was successfully inserted into the queue.
   *  False otherwise.
   * */
  bool push(const T& value);

  /**
   *  @brief Puses a new element into the queue.
   *  @param value The new value to be inserted into the queue. It will be
   *  `moved` on insertion.
   *  @returns True if the value was successfully inserted into the queue.
   *  False otherwise.
   * */
  bool push(T&& value);

  /**
   *  @returns A pointer to the element at the front of the queue. If the queue
   *  is empty, nullprt will be returned.
   * */
  T* front();

  /**
   *  @brief Deletes the element at the front of the queue. If there is no
   *  element, this function does nothing.
   * */
  void pop();

  /**
   *  @returns The maximun size of the queue.
   * */
  std::size_t max_size() const { return Capacity; }

  /**
   *  @return The number of elements left in the queue.
   * */
  std::size_t size() const { return this->space_used_; };
private:
  std::array<T, Capacity> queue_;
  std::size_t head_ = 0;
  std::size_t tail_ = 0;
  std::size_t space_used_ = 0;
};
}


#include "circular_queue_impl.hpp"

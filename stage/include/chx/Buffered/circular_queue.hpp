#pragma once

#include <array>

namespace chx::buffered {
template <typename T, std::size_t Capacity>
  requires(Capacity > 0)
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
  bool push(const T &value);

  /**
   *  @brief Pushes a new element into the queue.
   *  @param value The new value to be inserted into the queue. It will be
   *  `moved` on insertion.
   *  @returns True if the value was successfully inserted into the queue.
   *  False otherwise.
   * */
  bool push(T &&value);

  /**
   *  @returns A pointer to the element at the front of the queue. If the queue
   *  is empty, nullprt will be returned.
   * */
  T *front();

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

  /**
   *  @return True if the queue is full. False otherwise.
   * */
  bool is_full() const { return this->space_used_ == Capacity; }

  /**
   *  @return True if the queue is empty. False otherwise.
   * */
  bool is_empty() const { return this->space_used_ == 0; }

private:
  std::array<T, Capacity> queue_;
  std::size_t head_ = 0;
  std::size_t tail_ = 0;
  std::size_t space_used_ = 0;
};

template <typename T, std::size_t Capacity>
  requires(Capacity > 0)
bool CircularQueue<T, Capacity>::push(const T &value) {
  if (this->is_full()) {
    return false;
  }
  if (this->is_empty()) {
    this->queue_[this->head_] = value;
    this->tail_ = this->head_;
  } else {
    this->queue_[this->tail_] = value;
  }
  this->tail_ = (this->tail_ + 1) % Capacity;
  this->space_used_++;
  return true;
}

template <typename T, std::size_t Capacity>
  requires(Capacity > 0)
bool CircularQueue<T, Capacity>::push(T &&value) {
  if (this->is_full()) {
    return false;
  }
  if (this->space_used_ == 0) {
    this->queue_[this->head_] = std::move(value);
  } else {
    this->queue_[this->tail_] = std::move(value);
  }
  this->tail_ = (this->tail_ + 1) % Capacity;
  this->space_used_++;
  return true;
}

template <typename T, std::size_t Capacity>
  requires(Capacity > 0)
T *CircularQueue<T, Capacity>::front() {
  if (this->is_empty()) {
    return nullptr;
  }
  return &this->queue_[this->head_];
}

template <typename T, std::size_t Capacity>
  requires(Capacity > 0)
void CircularQueue<T, Capacity>::pop() {
  if (this->is_empty()) {
    return;
  }
  this->head_ = (this->head_ + 1) % Capacity;
  this->space_used_--;
  return;
}
} // namespace chx::buffered

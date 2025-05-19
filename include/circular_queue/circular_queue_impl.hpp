#pragma once

#include "circular_queue.hpp"

namespace chx {
template <typename T, std::size_t Capacity>
requires (Capacity > 0)
bool CircularQueue<T, Capacity>::push(const T& value) {
  if (this->space_used_ == Capacity) {
    return false;
  }
  if (this->space_used_ == 0) {
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
requires (Capacity > 0)
bool CircularQueue<T, Capacity>::push(T&& value) {
  if (this->space_used_ == Capacity) {
    return false;
  }
  if (this->space_used_ == 0) {
    this->queue_[this->head_] = std::move(value);
  } else {
    this->queue_[this->tail_] = std::move(value);
  }
  this->tail_ = (this->tail_ + 1) % Capacity;
  this->space_used_++;
}

template <typename T, std::size_t Capacity>
requires (Capacity > 0)
T* CircularQueue<T, Capacity>::front() {
  if (this->space_used_ == 0) {
    return nullptr;
  }
  return &this->queue_[this->head_];
}

template <typename T, std::size_t Capacity>
requires (Capacity > 0)
void CircularQueue<T, Capacity>::pop() {
  if (this->space_used_ == 0) {
    return;
  }
  this->head_ = (this->head_ + 1) % Capacity;
  this->space_used_--;
  return;
}
} // namespace chx

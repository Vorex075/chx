#pragma once

#include "circular_queue/circular_queue.hpp"
#include <condition_variable>
#include <mutex>
namespace chx::buffered {

template <class T, std::size_t Capacity> struct DataHandler {
  mutable std::mutex mutex;
  std::condition_variable not_empty;
  std::condition_variable not_full;
  CircularQueue<T, Capacity> queue;
  bool closed = false;
};

} // namespace chx::buffered

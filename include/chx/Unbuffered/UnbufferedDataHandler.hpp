#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>

namespace chx::unbuffered {
template <typename T> struct DataHandler {
  mutable std::mutex mutex;
  std::condition_variable sender_entrance;
  std::condition_variable sender_exit;
  std::condition_variable receiver_entrance;
  bool value_set = false;
  std::optional<T> slot;
  bool closed = false;
  unsigned int receivers_waiting = 0;
};

} // namespace chx::unbuffered

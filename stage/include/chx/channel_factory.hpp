#pragma once

#include "chx/Buffered/BufferedChannel.hpp"
#include "chx/Unbuffered/UnbufferedChannel.hpp"
#include "chx/channel.hpp"

namespace chx {
template <typename T, std::size_t Capacity = 0>
Channel<T> CreateChannel()
  requires(Capacity != 0)
{
  std::shared_ptr<ChannelCore<T>> core =
      std::make_shared<buffered::Channel<T, Capacity>>();
  return Channel<T>(core);
}

template <typename T, std::size_t Capacity = 0>
Channel<T> CreateChannel()
  requires(Capacity == 0)
{
  std::shared_ptr<ChannelCore<T>> core =
      std::make_shared<unbuffered::Channel<T>>();
  return Channel<T>(core);
}

} // namespace chx

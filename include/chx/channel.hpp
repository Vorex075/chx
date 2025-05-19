#ifndef CHANNEL_H
#define CHANNEL_H

#include <array>

namespace chx {
template <typename T, std::size_t size>
class Channel {

  explicit Channel();

  void send(T& value);

  T receive(T& value);

  void close();
  bool is_closed() const;
};
}


#endif

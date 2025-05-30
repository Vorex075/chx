#pragma once

#include <expected>
#include <string>

namespace chx {

typedef std::string Error;

template <typename T> class Channel {
public:
  Channel() = default;
  virtual ~Channel() = default;

  /**
   *  @brief Sends an object through the channel. This method blocks the thread
   * until the operation is done.
   *  @param value The object to be sent through the channel.
   *  @returns `void` if the operation was successful. An `Error` if the
   * operation failed.
   * */
  virtual std::expected<void, Error> send(T &&value) = 0;
  virtual std::expected<void, Error> send(const T &value) = 0;

  /**
   *  @brief Sends an object through the channel. This method does not block
   * the thread, so an error will be returned if the operation could not be done
   * inmediately.
   *  @param value The object to be sent through the channel.
   *  @returns `void` if the operation wasa successful. An `Error` if the
   * operation failed.
   * */
  virtual std::expected<void, Error> try_send(T &&value) = 0;
  virtual std::expected<void, Error> try_send(const T &value) = 0;

  /**
   *  @brief Receives an object through the channel. This method blocks the
   * thread until the operation is done.
   *  @returns An object (the one received from the channel), or an `Error` if
   * the operation failed.
   * */
  virtual std::expected<T, Error> receive() = 0;

  /**
   *  @brief Receives an object through the channel. This method does not block
   *  the thread, so an error will be returned if the operation could not be
   * done inmediately.
   *  @param value The object to be sent through the channel.
   *  @returns An object (the one received from the channel), or an `Error` if
   * the operation failed.
   * */
  virtual std::expected<T, Error> try_receive() = 0;

  virtual void close() = 0;
  virtual bool is_closed() const = 0;
};

} // namespace chx

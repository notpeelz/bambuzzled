#pragma once

#include <optional>
#include <stdexcept>

#if defined(_WIN32)
#include <winsock2.h>
using native_socket_handle = SOCKET;
#else
#include <unistd.h>
using native_socket_handle = int;
#endif

namespace bbzl::utils::net {

class socket_handle {
private:
  std::optional<native_socket_handle> handle_ = std::nullopt;

public:
  socket_handle() noexcept {}
  socket_handle(native_socket_handle handle) noexcept
    : handle_(handle) {}

  ~socket_handle() noexcept {
    if (this->handle_.has_value()) {
      #if defined(_WIN32)
      (void)closesocket(this->handle_.value());
      #else
      (void)close(this->handle_.value());
      #endif
    }
  }

  socket_handle(const socket_handle& rhs) = delete;
  socket_handle& operator=(const socket_handle& rhs) = delete;

  socket_handle(socket_handle&& rhs) noexcept
    : handle_(rhs.handle_)
  {
    rhs.handle_ = std::nullopt;
  }
  socket_handle& operator=(socket_handle&& rhs) noexcept {
    if (&rhs == this) {
      return *this;
    }

    this->handle_ = rhs.handle_;
    rhs.handle_ = std::nullopt;
    return *this;
  }

  bool is_valid() const noexcept {
    return this->handle_.has_value();
  }

  native_socket_handle get() const {
    if (!is_valid()) {
      throw std::logic_error("invalid handle");
    }
    return this->handle_.value();
  }

  operator native_socket_handle() const {
    return this->get();
  }
};

}

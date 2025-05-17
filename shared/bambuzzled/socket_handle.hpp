#pragma once

#include <optional>

#if defined(_WIN32)
#include <winsock2.h>
using native_socket_handle = SOCKET;
#else
#include <unistd.h>
using native_socket_handle = int;
#endif

namespace bbzl::utils::socket {

class socket_handle {
private:
  std::optional<native_socket_handle> handle_ = std::nullopt;

public:
  socket_handle() {}
  socket_handle(native_socket_handle fd)
    : handle_(fd) {}

  ~socket_handle() {
    if (this->handle_.has_value()) {
      #if defined(_WIN32)
      (void)closesocket(this->handle_.value());
      #else
      (void)close(this->handle_.value());
      #endif
    }
  }

  socket_handle(const socket_handle& a) = delete;
  socket_handle& operator=(const socket_handle& a) = delete;

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

  native_socket_handle get() const {
    return this->handle_.value();
  }

  bool is_valid() const {
    return this->handle_.has_value();
  }

  operator native_socket_handle() const {
    return this->get();
  }
};

}

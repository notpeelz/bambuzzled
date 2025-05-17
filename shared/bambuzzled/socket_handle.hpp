#pragma once

#if defined(_WIN32)
#include <winsock2.h>
using native_socket_handle = SOCKET;
constexpr native_socket_handle INVALID_SOCKET_VALUE = INVALID_SOCKET;
#else
using native_socket_handle = int;
constexpr native_socket_handle INVALID_SOCKET_VALUE = -1;
#endif

namespace bbzl::utils {

class socket_handle {
private:
  native_socket_handle handle_;
public:
  socket_handle() : handle_(INVALID_SOCKET_VALUE) {}

  socket_handle(native_socket_handle fd) : handle_(fd) {}

  socket_handle(const socket_handle& a) {
    this->handle_ = a.handle_;
  }

  socket_handle(socket_handle&& a) noexcept
    : handle_(a.handle_)
  {
    a.handle_ = -1;
  }

  ~socket_handle() {
    if (this->handle_ >= 0) {
      (void)closesocket(this->handle_);
    }
  }

  socket_handle& operator=(const socket_handle& a) {
    if (&a == this) {
      return *this;
    }

    this->handle_ = a.handle_;
    return *this;
  }

  socket_handle& operator=(socket_handle&& a) noexcept {
    if (&a == this) {
      return *this;
    }

    this->handle_ = a.handle_;
    a.handle_ = INVALID_SOCKET_VALUE;
    return *this;
  }

  native_socket_handle get() const {
    return this->handle_;
  }

  operator native_socket_handle() const {
    return this->get();
  }
};

}

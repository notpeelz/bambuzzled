#pragma once

#if !defined(_WIN32)

#include <unistd.h>

namespace bbzl::utils {

class fd_handle {
private:
  int fd_;
public:
  fd_handle() : fd_(-1) {}
  fd_handle(int fd) : fd_(fd) {}
  fd_handle(fd_handle&& a) noexcept
    : fd_(a.fd_)
  {
    a.fd_ = -1;
  }

  fd_handle(const fd_handle& a) = delete;
  fd_handle& operator=(const fd_handle& a) = delete;

  ~fd_handle() {
    if (this->fd_ >= 0) {
      (void)close(this->fd_);
    }
  }

  fd_handle& operator=(fd_handle&& a) noexcept {
    if (&a == this) {
      return *this;
    }

    this->fd_ = a.fd_;
    a.fd_ = -1;
    return *this;
  }

  int get() const {
    return this->fd_;
  }

  operator int() const { return this->get(); }
};

}

#endif

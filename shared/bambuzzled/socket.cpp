#include <span>
#include <tuple>
#include <string>
#include <format>
#include <utility>
#include <cstddef>
#include <stdexcept>
#include <bambuzzled/socket_handle.hpp>
#include "socket.hpp"

#if defined(_WIN32)
#include <winsock2.h>
#include <winerror.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#include <bambuzzled/win32.hpp>
#else
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

namespace bbzl::utils::socket {

class socket_err_code {
private:
  int value_ = 0;

public:
  socket_err_code(int value)
    : value_(value) {}

  bool eintr() {
    #if defined(_WIN32)
    return value_ == WSAEINTR;
    #else
    return value_ == EINTR;
    #endif
  }

  int get() const {
    return this->value_;
  }

  operator int() const {
    return this->get();
  }
};

static std::tuple<socket_err_code, std::string> get_socket_err() {
  #if defined(_WIN32)
  auto err = ::WSAGetLastError();
  auto err_msg = bbzl::utils::win32::get_err_msg(err);
  #else
  auto err = errno;
  auto err_msg = std::strerror(err);
  #endif
  return std::make_tuple(socket_err_code(err), err_msg);
}

static std::string get_socket_err_msg() {
  auto [err, err_msg] = get_socket_err();
  return std::move(err_msg);
}

void socket::set_recv_timeout(int seconds) {
  int r;
  bool errored;

  #if defined(_WIN32)
  r = ::setsockopt(
    this->handle_,
    SOL_SOCKET,
    SO_RCVTIMEO,
    reinterpret_cast<char const*>(&seconds),
    sizeof(seconds)
  );
  errored = r == SOCKET_ERROR;
  #else
  timeval rcvtimeout_tv = {
    .tv_sec = seconds,
  };
  r = ::setsockopt(
    this->handle_,
    SOL_SOCKET,
    SO_RCVTIMEO,
    &rcvtimeout_tv,
    sizeof(rcvtimeout_tv)
  );
  errored = r < 0;
  #endif

  if (errored) {
    throw std::runtime_error(std::format(
      "failed to set socket receive timeout: {}",
      get_socket_err_msg()
    ));
  }
}

void socket::set_send_timeout(int seconds) {
  int r;
  bool errored;

  #if defined(_WIN32)
  r = ::setsockopt(
    this->handle_,
    SOL_SOCKET,
    SO_SNDTIMEO,
    reinterpret_cast<char const*>(&seconds),
    sizeof(seconds)
  );
  errored = r == SOCKET_ERROR;
  #else
  timeval sndtimeout_tv = {
    .tv_sec = seconds,
  };
  r = ::setsockopt(
    this->handle_,
    SOL_SOCKET,
    SO_SNDTIMEO,
    &sndtimeout_tv,
    sizeof(sndtimeout_tv)
  );
  errored = r < 0;
  #endif

  if (errored) {
    throw std::runtime_error(std::format(
      "failed to set socket send timeout: {}",
      get_socket_err_msg()
    ));
  }
}

void socket::connect(sockaddr const& addr) {
  using sa_union = union {
    sockaddr sa;
    sockaddr_in s4;
    sockaddr_in6 s6;
  };

  int r;
  auto& sa = *reinterpret_cast<sa_union const*>(&addr);
  auto sock_handle = socket_handle(
    ::socket(addr.sa_family, SOCK_STREAM, IPPROTO_TCP)
  );

  if (!sock_handle.is_valid()) {
    throw std::runtime_error(std::format(
      "failed to create socket: {}",
      get_socket_err_msg()
    ));
  }

  bool errored;
  while (true) {
    #if defined(_WIN32)
    r = ::connect(
      sock_handle,
      &sa.sa,
      sizeof(sa)
    );
    errored = r == SOCKET_ERROR;
    #else
    r = ::connect(
      sock_handle,
      &sa.sa,
      sizeof(sa)
    );
    errored = r < 0;
    #endif

    if (errored) {
      auto [err, err_msg] = get_socket_err();
      if (err.eintr()) {
        continue;
      }

      throw std::runtime_error(std::format(
        "can't connect: {}",
        err_msg
      ));
    }

    break;
  }

  this->handle_ = std::move(sock_handle);
}

int socket::send(std::span<std::byte> const& data) {
  int r;
  bool errored;
  while (true) {
    #if defined(_WIN32)
    r = ::send(
      this->handle_,
      reinterpret_cast<char*>(data.data()),
      data.size(),
      0
    );
    errored = r == SOCKET_ERROR;
    #else
    r = ::write(this->handle_, data.data(), data.size());
    errored = r < 0;
    #endif

    if (errored) {
      auto [err, err_msg] = get_socket_err();
      if (err.eintr()) {
        continue;
      }

      throw std::runtime_error(std::format(
        "failed to send message: {}",
        err_msg
      ));
    }

    return r;
  }
}

int socket::receive(std::span<std::byte>& buf) {
  int r;
  bool errored;
  while (true) {
    #if defined(_WIN32)
    r = ::recv(
      this->handle_,
      reinterpret_cast<char*>(buf.data()),
      buf.size_bytes(),
      0
    );
    errored = r == SOCKET_ERROR;
    #else
    r = ::recv(this->handle_, buf.data(), buf.size_bytes(), 0);
    errored = r < 0;
    #endif

    if (errored) {
      auto [err, err_msg] = get_socket_err();
      if (err.eintr()) {
        continue;
      }

      throw std::runtime_error(std::format(
        "failed to receive message: {}",
        err_msg
      ));
    }

    return r;
  }
}

}

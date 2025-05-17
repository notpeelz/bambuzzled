#include <span>
#include <format>
#include <cerrno>
#include <utility>
#include <cstring>
#include <cstddef>
#include <stdexcept>
#include <bambuzzled/socket_handle.hpp>
#include "socket.hpp"

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

namespace bbzl::utils::socket {

void socket::set_recv_timeout(int seconds) {
  int r;

  #if !defined(_WIN32)
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
  #else
  r = ::setsockopt(
    this->handle_,
    SOL_SOCKET,
    SO_SNDTIMEO,
    reinterpret_cast<char const*>(&seconds),
    sizeof(seconds)
  );
  #endif

  if (r < 0) {
    throw std::runtime_error(std::format(
      "failed to set SO_SNDTIMEO: {}",
      std::strerror(errno)
    ));
  }
}

void socket::set_send_timeout(int seconds) {
  int r;

  #if !defined(_WIN32)
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
  #else
  r = ::setsockopt(
    this->handle_,
    SOL_SOCKET,
    SO_RCVTIMEO,
    reinterpret_cast<char const*>(&seconds),
    sizeof(seconds)
  );
  #endif

  if (r < 0) {
    throw std::runtime_error(std::format(
      "failed to set SO_RCVTIMEO: {}",
      std::strerror(errno)
    ));
  }
}

void socket::connect(sockaddr const& addr, size_t addr_len) {
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
    auto err = std::strerror(errno);
    throw std::runtime_error(std::format(
      "failed to create socket: {}",
      err
    ));
  }

  r = ::connect(
    sock_handle,
    &sa.sa,
    sizeof(sa)
  );
  if (r < 0) {
    throw std::runtime_error(std::format(
      "can't connect: {}",
      std::strerror(errno)
    ));
  }

  this->handle_ = std::move(sock_handle);
}

int socket::send(std::span<std::byte> const& data) {
  int r;

  while (true) {
    #if defined(_WIN32)
    r = send(this->handle_, reinterpret_cast<char*>(data.data()), data.size(), 0);
    #else
    r = write(this->handle_, data.data(), data.size());
    #endif

    if (r < 0) {
      if (errno == EINTR) {
        continue;
      }

      throw std::runtime_error(std::format(
        "failed to send message: {}",
        std::strerror(errno)
      ));
    }

    return r;
  }
}

int socket::receive(std::span<std::byte>& buf) {
  int r;

  #if defined(_WIN32)
  auto buf_ptr = reinterpret_cast<char*>(buf.data());
  #else
  auto buf_ptr = buf.data();
  #endif

  while (true) {
    r = recv(this->handle_, buf_ptr, buf.size_bytes(), 0);
    if (r < 0) {
      if (errno == EINTR) {
        continue;
      }

      throw std::runtime_error(std::format(
        "failed to receive message: {}",
        std::strerror(errno)
      ));
    }

    return r;
  }
}

}

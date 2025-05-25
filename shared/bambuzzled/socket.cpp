#include <span>
#include <tuple>
#include <string>
#include <format>
#include <cerrno>
#include <cstring>
#include <utility>
#include <cstddef>
#include <stdexcept>
#include <openssl/bio.h>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/log.hpp>
#include <bambuzzled/socket_handle.hpp>
#include "socket.hpp"

#if defined(_WIN32)
#include <winsock2.h>
#include <winerror.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#include <bambuzzled/win32.hpp>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

namespace bbzl::utils::net {

class socket_err_code {
private:
  int value_ = 0;

public:
  socket_err_code(int value) noexcept
    : value_(value) {}

  bool eintr() const noexcept {
    #if defined(_WIN32)
    return value_ == WSAEINTR;
    #else
    return value_ == EINTR;
    #endif
  }

  int get() const noexcept {
    return this->value_;
  }

  operator int() const noexcept {
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

std::string sockaddr_to_string(
  sockaddr const& addr,
  socklen_t addr_len,
  bool with_port
) {
  using sa_union = union {
    sockaddr sa;
    sockaddr_in s4;
    sockaddr_in6 s6;
  };
  auto& sa = *reinterpret_cast<sa_union const*>(&addr);
  void const* data = nullptr;
  #if defined(_WIN32)
  USHORT const* port = nullptr;
  #else
  in_port_t const* port = nullptr;
  #endif

  switch (sa.sa.sa_family) {
    case AF_INET: {
      data = reinterpret_cast<void const*>(&sa.s4.sin_addr);
      port = &sa.s4.sin_port;
      break;
    }
    case AF_INET6: {
      data = reinterpret_cast<void const*>(&sa.s6.sin6_addr);
      port = &sa.s6.sin6_port;
      break;
    }
    default: {
      throw std::logic_error(std::format(
        "unsupported address family: {}",
        sa.sa.sa_family
      ));
      break;
    }
  }

  char host[256] = {};
  auto r = inet_ntop(addr.sa_family, data, host, sizeof(host));
  if (r == nullptr) {
    throw std::runtime_error(std::format(
      "failed to convert address to string: {}",
      std::strerror(errno)
    ));
  }

  if (with_port) {
    return std::format("{}:{}", host, *port);
  }

  return std::string(host);
}

void socket::set_recv_timeout(int seconds) {
  int r;
  bool errored;

  #if defined(_WIN32)
  r = ::setsockopt(
    this->handle_.native_handle(),
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
    this->handle_.native_handle(),
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
    this->handle_.native_handle(),
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
    this->handle_.native_handle(),
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

void socket::connect(sockaddr const& addr, socklen_t addr_len) {
  bbzl::log::trace_fmt(
    "creating socket (family: {})",
    addr.sa_family
  );

  int r;
  int sockfd = ::BIO_socket(
    addr.sa_family,
    SOCK_STREAM,
    IPPROTO_TCP,
    0
  );
  auto sock_handle = bbzl::utils::net::openssl_bio_handle(
    ::BIO_new_socket(sockfd, BIO_CLOSE)
  );

  if (!sock_handle.is_valid()) {
    throw std::runtime_error(std::format(
      "failed to create socket: {}",
      get_socket_err_msg()
    ));
  }

  auto addr_str = sockaddr_to_string(addr, addr_len);
  bbzl::log::trace_fmt("({}) connecting socket", addr_str);

  bool errored;
  while (true) {
    #if defined(_WIN32)
    r = ::connect(
      sock_handle.native_handle(),
      &addr,
      addr_len
    );
    errored = r == SOCKET_ERROR;
    #else
    r = ::connect(
      sock_handle.native_handle(),
      &addr,
      addr_len
    );
    errored = r < 0;
    #endif

    if (errored) {
      auto [err, err_msg] = get_socket_err();
      if (err.eintr()) {
        continue;
      }

      throw socket_connect_failed(err_msg);
    }

    break;
  }

  this->handle_ = std::move(sock_handle);
  this->addr_str_ = std::move(addr_str);
}

int socket::send(std::span<std::byte> const& data) {
  int r;

  bbzl::log::trace_fmt(
    "({}) writing to socket ({} bytes)",
    this->addr_str_,
    data.size_bytes()
  );
  while (true) {
    r = ::BIO_write(this->handle_, data.data(), data.size_bytes());

    if (r <= 0) {
      if (BIO_should_retry(this->handle_)) {
        bbzl::log::trace_fmt(
          "({}) retrying socket write",
          this->addr_str_
        );
        continue;
      }

      throw socket_send_failed(
        bbzl::utils::net::get_ssl_error_messages()
      );
    }

    return r;
  }
}

int socket::receive(std::span<std::byte>& buf) {
  int r;

  bbzl::log::trace_fmt("({}) reading from socket", this->addr_str_);
  while (true) {
    r = ::BIO_read(this->handle_, buf.data(), buf.size_bytes());

    if (r <= 0) {
      if (BIO_should_retry(this->handle_)) {
        bbzl::log::trace_fmt(
          "({}) retrying socket read",
          this->addr_str_
        );
        continue;
      }

      throw socket_recv_failed(
        bbzl::utils::net::get_ssl_error_messages()
      );
    }

    return r;
  }
}

}

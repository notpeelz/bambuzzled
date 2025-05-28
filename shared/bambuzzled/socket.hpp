#pragma once

#include <span>
#include <format>
#include <string>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <functional>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/socket_handle.hpp>
#include <bambuzzled/optional_ref.hpp>

#if defined(_WIN32)
#include <windows.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

namespace bbzl::utils::net {

#if defined(_WIN32)
using net_port = USHORT;
#else
using net_port = in_port_t;
#endif

class socket_error
  : public std::runtime_error
{
private:
  std::string reason_;

public:
  socket_error(
    std::string const& msg,
    std::string const& reason
  )
    : std::runtime_error(std::format("{}: {}", msg, reason)),
      reason_(reason) {}

  socket_error(std::string const& msg)
    : std::runtime_error(msg) {}

  [[nodiscard]]
  bbzl::utils::optional_ref<std::string const> reason() {
    return std::make_optional(std::cref(this->reason_));
  }
};

class invalid_address
  : public socket_error
{
private:
  std::string reason_;
  static constexpr char const msg[] = "invalid address";

public:
  invalid_address(std::string const& reason)
    : socket_error(msg, reason) {}

  invalid_address()
    : socket_error(msg) {}
};

std::string sockaddr_to_string(
  sockaddr const& addr,
  socklen_t addr_len,
  bool with_port = true
);

sockaddr_storage string_to_sockaddr(
  std::string const& s,
  bool with_port
);

class socket_connect_failed
  : public socket_error
{
protected:
  struct raw_msg_tag {};

  socket_connect_failed(raw_msg_tag, std::string const& msg)
    : socket_error(msg) {}

  socket_connect_failed(
    raw_msg_tag,
    std::string const& msg,
    std::string reason
  )
    : socket_error(msg) {}

private:
  static constexpr char const msg[] = "socket failed to connect";

public:
  socket_connect_failed(std::string const& reason)
    : socket_error(msg, reason) {}

  socket_connect_failed()
    : socket_error(msg) {}
};

class socket_send_failed
  : public socket_error
{
protected:
  struct raw_msg_tag {};

  socket_send_failed(raw_msg_tag, std::string const& msg)
    : socket_error(msg) {}

  socket_send_failed(
    raw_msg_tag,
    std::string const& msg,
    std::string reason
  )
    : socket_error(msg) {}

private:
  static constexpr char const msg[] = "socket failed to send";

public:
  socket_send_failed(std::string const& reason)
    : socket_error(msg, reason) {}

  socket_send_failed()
    : socket_error(msg) {}
};

class socket_recv_failed
  : public socket_error
{
protected:
  struct raw_msg_tag {};
  socket_recv_failed(raw_msg_tag, std::string const& msg)
    : socket_error(msg) {}

  socket_recv_failed(
    raw_msg_tag,
    std::string const& msg,
    std::string reason
  )
    : socket_error(msg, reason) {}

private:
  static constexpr char const msg[] = "socket failed to receive";

public:
  socket_recv_failed(std::string const& reason)
    : socket_error(msg, reason) {}

  socket_recv_failed()
    : socket_error(msg) {}
};

class socket {
protected:
  #if defined(_WIN32)
  using socket_handle =
    bbzl::utils::net::openssl_bio_native_handle<SOCKET>;
  #else
  using socket_handle =
    bbzl::utils::net::openssl_bio_native_handle<int>;
  #endif

  socket_handle handle_;
  std::string addr_str_;

public:
  socket() {}
  virtual ~socket() {}

  void set_recv_timeout(int seconds);
  void set_send_timeout(int seconds);

  virtual void connect(sockaddr const& addr, socklen_t addr_len);
  void connect(sockaddr_in const& addr) {
    this->connect(
      *reinterpret_cast<sockaddr const*>(&addr),
      sizeof(addr)
    );
  }
  void connect(sockaddr_in6 const& addr) {
    this->connect(
      *reinterpret_cast<sockaddr const*>(&addr),
      sizeof(addr)
    );
  }

  virtual int send(std::span<std::byte> const& data);

  [[nodiscard]]
  virtual int receive(std::span<std::byte>& buf);
};

}

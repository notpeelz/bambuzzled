#pragma once

#include <span>
#include <string>
#include <cstddef>
#include <stdexcept>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/socket_handle.hpp>

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

class invalid_address
  : public std::runtime_error
{
public:
  invalid_address(std::string const& s)
    : std::runtime_error("invalid address: " + s) {}
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
  : public std::runtime_error
{
protected:
  std::string reason_;

  struct raw_msg_tag {};
  socket_connect_failed(raw_msg_tag, std::string const& msg)
    : std::runtime_error(msg) {}
  socket_connect_failed(
    raw_msg_tag,
    std::string const& msg,
    std::string reason
  )
    : std::runtime_error(msg),
    reason_(reason) {}

public:
  socket_connect_failed()
    : std::runtime_error("socket failed to connect") {}

  socket_connect_failed(std::string const& reason)
    : std::runtime_error("socket failed to connect: " + reason),
    reason_(reason) {}

  [[nodiscard]]
  std::string& reason() {
    return this->reason_;
  }
};

class socket_send_failed
  : public std::runtime_error
{
protected:
  std::string reason_;

  struct raw_msg_tag {};
  socket_send_failed(raw_msg_tag, std::string const& msg)
    : std::runtime_error(msg) {}
  socket_send_failed(
    raw_msg_tag,
    std::string const& msg,
    std::string reason
  )
    : std::runtime_error(msg),
    reason_(reason) {}

public:
  socket_send_failed()
    : std::runtime_error("socket failed to send") {}

  socket_send_failed(std::string const& reason)
    : std::runtime_error("socket failed to send: " + reason),
    reason_(reason) {}

  [[nodiscard]]
  std::string& reason() {
    return this->reason_;
  }
};

class socket_recv_failed
  : public std::runtime_error
{
protected:
  std::string reason_;

  struct raw_msg_tag {};
  socket_recv_failed(raw_msg_tag, std::string const& msg)
    : std::runtime_error(msg) {}
  socket_recv_failed(
    raw_msg_tag,
    std::string const& msg,
    std::string reason
  )
    : std::runtime_error(msg),
    reason_(reason) {}

public:
  socket_recv_failed()
    : std::runtime_error("socket failed to receive") {}

  socket_recv_failed(std::string const& reason)
    : std::runtime_error("socket failed to receive: " + reason),
    reason_(reason) {}

  [[nodiscard]]
  std::string& reason() {
    return this->reason_;
  }
};

class socket {
protected:
  bbzl::utils::net::openssl_bio_handle handle_;
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

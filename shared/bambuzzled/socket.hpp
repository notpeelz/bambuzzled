#pragma once

#include <span>
#include <string>
#include <cstddef>
#include <stdexcept>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/socket_handle.hpp>

#if defined(_WIN32)
#include <ws2ipdef.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

namespace bbzl::utils::net {

std::string sockaddr_to_string(
  sockaddr const& addr,
  socklen_t addr_len,
  bool with_port = true
);

class socket_connect_failed
  : public std::runtime_error
{
protected:
  struct raw_msg_tag {};

  socket_connect_failed(raw_msg_tag, std::string const& msg)
    : std::runtime_error(msg) {}

public:
  socket_connect_failed()
    : std::runtime_error("socket failed to connect (SSL/TLS)") {}

  socket_connect_failed(std::string const& msg)
    : std::runtime_error("socket failed to connect (SSL/TLS): " + msg) {}
};

class socket_send_failed
  : public std::runtime_error
{
protected:
  struct raw_msg_tag {};

  socket_send_failed(raw_msg_tag, std::string const& msg)
    : std::runtime_error(msg) {}

public:
  socket_send_failed()
    : std::runtime_error("socket failed to send") {}

  socket_send_failed(std::string const& msg)
    : std::runtime_error("socket failed to send: " + msg) {}
};

class socket_recv_failed
  : public std::runtime_error
{
protected:
  struct raw_msg_tag {};

  socket_recv_failed(raw_msg_tag, std::string const& msg)
    : std::runtime_error(msg) {}

public:
  socket_recv_failed()
    : std::runtime_error("socket failed to receive") {}

  socket_recv_failed(std::string const& msg)
    : std::runtime_error("socket failed to receive: " + msg) {}
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

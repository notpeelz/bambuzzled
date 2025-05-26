#pragma once

#include <span>
#include <string>
#include <cstddef>
#include <openssl/crypto.h>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/socket.hpp>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

namespace bbzl::utils::net {

class socket_encrypted_connect_failed
  : public socket_connect_failed
{
public:
  socket_encrypted_connect_failed()
    : socket_connect_failed(
      socket_connect_failed::raw_msg_tag(),
      "socket_encrypted failed to connect"
    ) {}

  socket_encrypted_connect_failed(std::string const& reason)
    : socket_connect_failed(
      socket_connect_failed::raw_msg_tag(),
      "socket_encrypted failed to connect: " + reason,
      reason
    ) {}
};

class socket_encrypted_send_failed
  : public socket_send_failed
{
public:
  socket_encrypted_send_failed()
    : socket_send_failed(
      socket_send_failed::raw_msg_tag(),
      "socket_encrypted failed to send"
    ) {}

  socket_encrypted_send_failed(std::string const& reason)
    : socket_send_failed(
      socket_send_failed::raw_msg_tag(),
      "socket_encrypted failed to send: " + reason,
      reason
    ) {}
};

class socket_encrypted_recv_failed
  : public socket_recv_failed
{
public:
  socket_encrypted_recv_failed()
    : socket_recv_failed(
      socket_recv_failed::raw_msg_tag(),
      "socket_encrypted failed to receive"
    ) {}

  socket_encrypted_recv_failed(std::string const& reason)
    : socket_recv_failed(
      socket_recv_failed::raw_msg_tag(),
      "socket_encrypted failed to receive: " + reason,
      reason
    ) {}
};

class socket_encrypted
  : public bbzl::utils::net::socket
{
private:
  SSL_CTX* ssl_ctx_ = nullptr;
  bbzl::utils::net::openssl_ssl_handle ssl_;

public:
  socket_encrypted(SSL_CTX* ssl_ctx)
    : ssl_ctx_(ssl_ctx) {}
  ~socket_encrypted() {}

  void connect(sockaddr const& addr, socklen_t addr_len) override;
  int send(std::span<std::byte> const& data) override;
  int receive(std::span<std::byte>& buf) override;
};

}

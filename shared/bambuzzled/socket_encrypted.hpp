#pragma once

#include <netinet/in.h>
#include <span>
#include <cstddef>
#include <openssl/crypto.h>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/socket.hpp>
#include <sys/socket.h>

namespace bbzl::utils::socket {

class socket_encrypted
  : public bbzl::utils::socket::socket
{
private:
  SSL_CTX* ssl_ctx_;
  bbzl::ssl::openssl_ssl_handle ssl_;

public:
  socket_encrypted(SSL_CTX* ssl_ctx)
    : ssl_ctx_(ssl_ctx) {}
  ~socket_encrypted() = default;

  void connect(sockaddr const& addr, size_t addr_len) override;
  int send(std::span<std::byte> const& data) override;
  int receive(std::span<std::byte>& buf) override;
};

}

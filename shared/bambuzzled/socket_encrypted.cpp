#include <span>
#include <format>
#include <cerrno>
#include <cstddef>
#include <stdexcept>
#include <openssl/ssl.h>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/socket.hpp>
#include <sys/socket.h>
#include <utility>
#include "socket_encrypted.hpp"

namespace bbzl::utils::socket {

void socket_encrypted::connect(
  sockaddr const& addr,
  size_t addr_len
) {
  int r;

  socket::connect(addr, addr_len);

  auto ssl = bbzl::ssl::openssl_ssl_handle(SSL_new(this->ssl_ctx_));
  if (ssl == nullptr) {
    throw std::runtime_error(std::format(
      "failed to create SSL: {}",
      bbzl::ssl::get_err_msg()
    ));
  }

  #if defined(_WIN32)
  r = SSL_set_fd(ssl, static_cast<intptr_t>(this->handle_));
  #else
  r = SSL_set_fd(ssl, this->handle_);
  #endif

  if (r != 1) {
    throw std::runtime_error(std::format(
      "failed to set SSL fd: {}",
      bbzl::ssl::get_err_msg()
    ));
  }

  r = SSL_connect(ssl);
  if (r != 1) {
    throw std::runtime_error(std::format(
      "TLS/SSL handshake failed: {}",
      bbzl::ssl::get_err_msg()
    ));
  }

  this->ssl_ = std::move(ssl);
}

int socket_encrypted::send(std::span<std::byte> const& data) {
  int r;

  while (true) {
    r = SSL_write(this->ssl_, data.data(), data.size());
    if (r < 0) {
      if (errno == EINTR) {
        continue;
      }

      throw std::runtime_error(std::format(
        "failed to send message (using SSL/TLS): {}",
        bbzl::ssl::get_err_msg()
      ));
    }

    return r;
  }
}

int socket_encrypted::receive(std::span<std::byte>& buf) {
  int r;
  while (true) {
    r = SSL_read(this->ssl_, buf.data(), buf.size_bytes());
    if (r < 0) {
      if (errno == EINTR) {
        continue;
      }

      throw std::runtime_error(std::format(
        "failed to receive message (using SSL/TLS): {}",
        bbzl::ssl::get_err_msg()
      ));
    }

    return r;
  }
}

}

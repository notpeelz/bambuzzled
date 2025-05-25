#include <span>
#include <format>
#include <cstddef>
#include <stdexcept>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/socket.hpp>
#include <utility>
#include "socket_encrypted.hpp"

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#endif

namespace bbzl::utils::net {

void socket_encrypted::connect(sockaddr const& addr, socklen_t addr_len) {
  int r;

  socket::connect(addr, addr_len);

  auto ssl = bbzl::utils::net::openssl_ssl_handle(
    SSL_new(this->ssl_ctx_)
  );
  if (ssl == nullptr) {
    throw socket_encrypted_connect_failed(std::format(
      "failed to create SSL: {}",
      bbzl::utils::net::get_ssl_error_messages()
    ));
  }

  auto socket_handle_ref = this->handle_.up_ref().into_raw();
  SSL_set_bio(ssl, socket_handle_ref, socket_handle_ref);

  while (true) {
    r = SSL_connect(ssl);
    if (r <= 0) {
      if (BIO_should_retry(this->handle_)) {
        continue;
      }

      throw socket_connect_failed(std::format(
        "TLS/SSL handshake failed: {}",
        bbzl::utils::net::get_ssl_error_messages()
      ));
    }

    break;
  }

  this->ssl_ = std::move(ssl);
}

int socket_encrypted::send(std::span<std::byte> const& data) {
  int r;

  while (true) {
    r = SSL_write(this->ssl_, data.data(), data.size());
    if (r <= 0) {
      if (BIO_should_retry(this->handle_)) {
        continue;
      }

      throw std::runtime_error(std::format(
        "failed to send message (using SSL/TLS): {}",
        bbzl::utils::net::get_ssl_error_messages()
      ));
    }

    return r;
  }
}

int socket_encrypted::receive(std::span<std::byte>& buf) {
  int r;
  while (true) {
    r = SSL_read(this->ssl_, buf.data(), buf.size_bytes());
    if (r <= 0) {
      if (BIO_should_retry(this->handle_)) {
        continue;
      }

      throw std::runtime_error(std::format(
        "failed to receive message (using SSL/TLS): {}",
        bbzl::utils::net::get_ssl_error_messages()
      ));
    }

    return r;
  }
}

}

#include <span>
#include <format>
#include <cstddef>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/socket.hpp>
#include <utility>
#include "socket_encrypted.hpp"

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
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
    throw socket_encrypted_connect_failed("failed to create ssl");
  }

  auto socket_handle_ref = this->handle_.up_ref().into_raw();
  SSL_set_bio(ssl, socket_handle_ref, socket_handle_ref);

  while (true) {
    r = SSL_connect(ssl);
    if (r <= 0) {
      if (BIO_should_retry(this->handle_)) {
        continue;
      }

      int err = SSL_get_error(this->ssl_, r);
      if (err != SSL_ERROR_NONE) {
        throw socket_encrypted_connect_failed(std::format(
          "ssl error code: {}",
          err
        ));
      }

      throw socket_encrypted_connect_failed();
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

      int err = SSL_get_error(this->ssl_, r);
      if (err != SSL_ERROR_NONE) {
        throw socket_encrypted_send_failed(std::format(
          "ssl error code: {}",
          err
        ));
      }

      throw socket_encrypted_send_failed();
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

      int err = SSL_get_error(this->ssl_, r);
      if (err != SSL_ERROR_NONE) {
        throw socket_encrypted_recv_failed(std::format(
          "ssl error code: {}",
          err
        ));
      }

      throw socket_encrypted_recv_failed();
    }

    return r;
  }
}

}

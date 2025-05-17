#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include <openssl/crypto.h>
#include <openssl/ssl.h>

namespace bbzl::ssl {

struct openssl_ctx_handle_deleter {
  void operator()(SSL_CTX* ctx) const {
    if (ctx != nullptr) {
      SSL_CTX_free(ctx);
    }
  }
};

class openssl_ctx_handle
  : public std::unique_ptr<SSL_CTX, openssl_ctx_handle_deleter>
{
public:
  using base = std::unique_ptr<SSL_CTX, openssl_ctx_handle_deleter>;
  using base::base;
  operator SSL_CTX*() const { return this->get(); }
};

struct openssl_ssl_handle_deleter {
  void operator()(SSL* ssl) const {
    if (ssl != nullptr) {
      SSL_free(ssl);
    }
  }
};

class openssl_ssl_handle
  : public std::unique_ptr<SSL, openssl_ssl_handle_deleter>
{
public:
  using base = std::unique_ptr<SSL, openssl_ssl_handle_deleter>;
  using base::base;
  operator SSL*() const { return this->get(); }
};

std::string get_err_msg() noexcept;

std::filesystem::path const& get_certs_dir() noexcept;

}

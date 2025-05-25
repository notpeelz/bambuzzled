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

using openssl_ctx_handle_base =
  std::unique_ptr<SSL_CTX, openssl_ctx_handle_deleter>;
class openssl_ctx_handle
  : public openssl_ctx_handle_base
{
public:
  using openssl_ctx_handle_base::openssl_ctx_handle_base;
  operator SSL_CTX*() const { return this->get(); }
};

struct openssl_ssl_handle_deleter {
  void operator()(SSL* ssl) const {
    if (ssl != nullptr) {
      SSL_free(ssl);
    }
  }
};

using openssl_ssl_handle_base =
  std::unique_ptr<SSL, openssl_ssl_handle_deleter>;
class openssl_ssl_handle
  : public openssl_ssl_handle_base
{
public:
  using openssl_ssl_handle_base::openssl_ssl_handle_base;
  operator SSL*() const { return this->get(); }
};

[[nodiscard]]
std::string pop_error_messages() noexcept;

[[nodiscard]]
std::filesystem::path const& get_certs_dir() noexcept;

}

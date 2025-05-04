#pragma once

#include <string>
#include <memory>

#include <openssl/ssl.h>
#include <openssl/err.h>

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

static std::string get_err_msg() {
  BIO* bio = BIO_new(BIO_s_mem());
  ERR_print_errors(bio);
  char* buf;
  size_t len = BIO_get_mem_data(bio, &buf);
  std::string s(buf, len);
  BIO_free(bio);
  return s;
}

}

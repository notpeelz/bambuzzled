#pragma once

#include <string>
#include <memory>

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace bbzl::utils {

struct openssl_ctx_deleter {
  void operator()(SSL_CTX* ctx) const {
    if (ctx != nullptr) {
      SSL_CTX_free(ctx);
    }
  }
};

using openssl_ctx_ptr = std::unique_ptr<SSL_CTX, openssl_ctx_deleter>;

static std::string get_openssl_err() {
  BIO* bio = BIO_new(BIO_s_mem());
  ERR_print_errors(bio);
  char* buf;
  size_t len = BIO_get_mem_data(bio, &buf);
  std::string s(buf, len);
  BIO_free(bio);
  return s;
}

}

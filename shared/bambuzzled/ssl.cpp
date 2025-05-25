#include <string>
#include <cstddef>
#include <filesystem>
#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include "ssl.hpp"

namespace bbzl::utils::net {

std::string get_ssl_error_messages() noexcept {
  auto bio = bbzl::utils::net::openssl_bio_handle(
    BIO_new(BIO_s_mem())
  );
  ERR_print_errors(bio);
  char* buf;
  size_t len = BIO_get_mem_data(bio, &buf);
  auto s = std::string(buf, len);
  return s;
}

std::filesystem::path const& get_certs_dir() noexcept {
  static std::filesystem::path certs_dir = BBZL_CERTS_DIR;
  return certs_dir;
}

}

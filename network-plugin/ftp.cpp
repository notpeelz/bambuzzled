#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <format>

#include <openssl/ssl.h>
#include <curl/curl.h>
#include <bambuzzled/log.hpp>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/curl.hpp>

#include "ftp.hpp"

namespace bbzl::ftp {

static CURLcode ssl_ctx_callback(CURL* curl, void* ssl_ctx, void* p) {
  (void)p;

  auto ctx = static_cast<SSL_CTX*>(ssl_ctx);
  SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

  X509_STORE* store = SSL_CTX_get_cert_store(ctx);
  X509_STORE_set_flags(store, X509_V_FLAG_NO_CHECK_TIME);

  X509_LOOKUP* lookup = X509_STORE_add_lookup(
    store,
    X509_LOOKUP_hash_dir()
  );
  X509_LOOKUP_add_dir(
    lookup,
    nullptr,
    X509_FILETYPE_DEFAULT
  );
  X509_LOOKUP_add_dir(
    lookup,
    reinterpret_cast<char const*>(
      bbzl::ssl::get_certs_dir().u8string().c_str()
    ),
    X509_FILETYPE_PEM
  );

  return CURLE_OK;
}

static size_t read_callback(char* buf, size_t size, size_t n, void* p) {
  auto& ifs = *static_cast<std::ifstream*>(p);
  ifs.read(buf, size * n);
  return ifs.gcount();
}

void upload_file(
  std::string const& url,
  std::string const& username,
  std::string const& password,
  std::filesystem::path file_path
) {
  std::ifstream ifs;
  ifs.open(file_path, std::ios::binary);
  if (!ifs.is_open()) {
    throw std::runtime_error(std::format(
      "failed to open file: {}",
      file_path.string()
    ));
  }

  auto curl = bbzl::curl::curl_handle(curl_easy_init());
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, ssl_ctx_callback);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
    curl_easy_setopt(curl, CURLOPT_PROTOCOLS_STR, "ftp,ftps");
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &ifs);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    CURLcode rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) {
      throw std::runtime_error(std::format(
        "failed to upload file: {}",
        curl_easy_strerror(rc)
      ));
    }
  }
}

}

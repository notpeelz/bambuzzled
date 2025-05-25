#include <ios>
#include <cstddef>
#include <iosfwd>
#include <string>
#include <format>
#include <optional>
#include <stdexcept>
#include <filesystem>
#include <curl/curl.h>
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/prov_ssl.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <bambuzzled/log.hpp>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/curl.hpp>
#include <bambuzzled/optional_ref.hpp>
#include "ftp.hpp"

namespace bbzl::ftp {

static CURLcode ssl_ctx_callback(
  CURL* curl,
  void* ssl_ctx,
  void* userdata
) noexcept {
  (void)userdata;

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
      bbzl::utils::net::get_certs_dir().u8string().c_str()
    ),
    X509_FILETYPE_PEM
  );

  return CURLE_OK;
}

static size_t read_callback(
  char* buf,
  size_t size,
  size_t n,
  void* p
) noexcept {
  auto& is = *static_cast<std::istream*>(p);
  is.read(buf, size * n);
  if (is.fail()) {
    bbzl::log::error_fmt(
      "failed while reading input stream for ftp upload"
    );
    return 0;
  }

  if (is.eof()) {
    return 0;
  }

  return is.gcount();
}

struct curl_xferinfo_data {
  std::optional<on_upload_progress_cb_fn> const& progress_cb;
};

static int xferinfo_callback(
  void* userdata,
  file_offset dltotal,
  file_offset dlnow,
  file_offset ultotal,
  file_offset ulnow
) noexcept {
  (void)dltotal;
  (void)dlnow;

  if (ultotal <= 0 || ulnow <= 0) {
    return CURLE_OK;
  }

  auto& data = *static_cast<curl_xferinfo_data const*>(userdata);
  if (data.progress_cb.has_value()) {
    (*data.progress_cb)(ultotal, ulnow);
  }
  return CURLE_OK;
}

bool upload_file(
  std::string const& url,
  std::string const& username,
  std::string const& password,
  std::istream& is,
  std::optional<on_upload_progress_cb_fn> const& on_upload_progess_cb,
  bbzl::utils::optional_ref<std::string> err
) {
  if (is.bad() || is.fail()) {
    throw std::invalid_argument("input stream isn't valid");
  }

  auto _ = is.tellg();
  if (is.fail()) {
    throw std::logic_error("input stream isn't seekable");
  }

  std::streampos is_size = 0;
  is_size = is.tellg();
  is.seekg(0, std::ios::end);
  is_size = is.tellg() - is_size;
  is.seekg(0, std::ios::beg);

  if (is.fail()) {
    throw std::runtime_error("failed to determine file size");
  }

  auto file_size = static_cast<curl_off_t>(is_size);
  auto userdata = curl_xferinfo_data {
    .progress_cb = on_upload_progess_cb,
  };

  auto curl = bbzl::utils::curl::curl_handle(curl_easy_init());
  if (curl == nullptr) {
    throw std::runtime_error("failed to create curl instance");
  }

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
  curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, file_size);
  curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &userdata);
  curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo_callback);
  curl_easy_setopt(curl, CURLOPT_READDATA, &is);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

  CURLcode rc = curl_easy_perform(curl);
  if (rc != CURLE_OK) {
    if (err.has_value()) {
      auto& err_ = err->get();
      err_ = curl_easy_strerror(rc);
    }
    return false;
  }

  return true;
}

}

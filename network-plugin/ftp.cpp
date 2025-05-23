#include <ios>
#include <cstddef>
#include <fstream>
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
      bbzl::ssl::get_certs_dir().u8string().c_str()
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
  auto& ifs = *static_cast<std::ifstream*>(p);
  ifs.read(buf, size * n);
  return ifs.gcount();
}

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

  auto& fn =
    *static_cast<std::optional<on_upload_progress_cb_fn> const*>(userdata);
  if (fn.has_value()) {
    (*fn)(ultotal, ulnow);
  }
  return CURLE_OK;
}

void upload_file(
  std::string const& url,
  std::string const& username,
  std::string const& password,
  std::filesystem::path file_path,
  std::optional<on_upload_progress_cb_fn> const& on_upload_progess_cb
) {
  std::ifstream ifs;
  ifs.open(file_path, std::ios::binary);
  if (!ifs.is_open()) {
    throw std::runtime_error(std::format(
      "failed to open file: {}",
      file_path.string()
    ));
  }

  std::streampos ifs_size = 0;
  ifs_size = ifs.tellg();
  ifs.seekg(0, std::ios::end);
  ifs_size = ifs.tellg() - ifs_size;
  ifs.seekg(0, std::ios::beg);
  auto file_size = static_cast<curl_off_t>(ifs_size);

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
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, file_size);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &on_upload_progess_cb);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &ifs);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

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

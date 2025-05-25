#pragma once

#include <string>
#include <memory>
#include <utility>
#include <optional>
#include <stdexcept>
#include <filesystem>
#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>

namespace bbzl::utils::net {

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

  bool is_valid() const noexcept {
    return this->get() != nullptr;
  }

  operator SSL_CTX*() const noexcept {
    return this->get();
  }
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

  bool is_valid() const noexcept {
    return this->get() != nullptr;
  }

  operator SSL*() const noexcept {
    return this->get();
  }
};

struct openssl_bio_handle_deleter {
  void operator()(BIO* bio) const {
    if (bio != nullptr) {
      BIO_free(bio);
    }
  }
};

class openssl_bio_handle {
private:
  std::shared_ptr<BIO> handle_;

  [[nodiscard]]
  std::optional<int> maybe_fd() const noexcept {
    if (this->to_raw() == nullptr) {
      return -1;
    }

    int fd;
    auto r = BIO_get_fd(this->to_raw(), &fd);
    if (r < 0) {
      return -1;
    }

    return fd;
  }
public:
  openssl_bio_handle() noexcept {}

  openssl_bio_handle(openssl_bio_handle const& o) noexcept {
    this->handle_ = o.handle_;
  }
  void swap(openssl_bio_handle& o) noexcept {
    o.handle_.swap(this->handle_);
  }
  openssl_bio_handle& operator=(openssl_bio_handle const& rhs) noexcept {
    openssl_bio_handle tmp(rhs);
    tmp.swap(*this);
    return *this;
  }

  openssl_bio_handle(openssl_bio_handle&& rhs) noexcept
    : handle_(std::move(rhs.handle_)) {}
  openssl_bio_handle& operator=(openssl_bio_handle&& rhs) noexcept {
    if (&rhs == this) {
      return *this;
    }

    this->handle_ = std::move(rhs.handle_);
    return *this;
  }

  openssl_bio_handle(BIO* o) noexcept {
    this->handle_ = std::shared_ptr<BIO>(
      o,
      openssl_bio_handle_deleter()
    );
  }

  operator BIO*() const noexcept {
    return this->to_raw();
  }

  [[nodiscard]]
  BIO* to_raw() const noexcept {
    return this->handle_.get();
  }

  [[nodiscard]]
  BIO* into_raw() && noexcept {
    BIO* bio = this->to_raw();
    this->handle_.reset();
    return bio;
  }

  [[nodiscard]]
  openssl_bio_handle up_ref() const {
    if (BIO_up_ref(this->to_raw()) != 1) {
      throw std::runtime_error("up_ref failed");
    }
    return openssl_bio_handle(*this);
  }

  [[nodiscard]]
  int native_handle() const {
    auto maybe_fd = this->maybe_fd();
    if (!maybe_fd.has_value()) {
      throw std::logic_error("bio handle is invalid");
    }

    return *maybe_fd;
  }

  [[nodiscard]]
  bool is_valid() const noexcept {
    return this->maybe_fd().has_value();
  }
};

[[nodiscard]]
std::string get_ssl_error_messages() noexcept;

[[nodiscard]]
std::filesystem::path const& get_certs_dir() noexcept;

}

#pragma once

#include <string>
#include <memory>
#include <utility>
#include <optional>
#include <stdexcept>
#include <filesystem>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>

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
protected:
  std::shared_ptr<BIO> handle_;

public:
  openssl_bio_handle() noexcept {}

  openssl_bio_handle(BIO* handle) noexcept
    : handle_(std::shared_ptr<BIO>(
      handle,
      openssl_bio_handle_deleter()
    )) {}

  openssl_bio_handle(openssl_bio_handle const& o) noexcept {
    this->handle_ = o.handle_;
  }
  void swap(openssl_bio_handle& o) noexcept {
    o.handle_.swap(this->handle_);
  }
  openssl_bio_handle& operator=(
    openssl_bio_handle const& rhs
  ) noexcept {
    openssl_bio_handle tmp(rhs);
    tmp.swap(*this);
    return *this;
  }

  openssl_bio_handle(openssl_bio_handle&& o) noexcept {
    this->handle_ = std::move(o.handle_);
  }
  openssl_bio_handle& operator=(openssl_bio_handle&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    this->handle_ = std::move(rhs.handle_);
    return *this;
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
  bool is_valid() const noexcept {
    return this->handle_ != nullptr;
  }
};

template<typename T>
class openssl_bio_native_handle
  : public openssl_bio_handle {
private:
  [[nodiscard]]
  std::optional<T> maybe_fd() const noexcept {
    if (this->to_raw() == nullptr) {
      return std::nullopt;
    }

    int fd;
    auto r = BIO_get_fd(this->to_raw(), &fd);
    if (r < 0) {
      return std::nullopt;
    }

    return static_cast<T>(fd);
  }

public:
  openssl_bio_native_handle() noexcept {}

  openssl_bio_native_handle(BIO* handle) noexcept
    : openssl_bio_handle(handle) {}

  openssl_bio_native_handle(
    openssl_bio_native_handle const& o
  ) noexcept {
    this->handle_ = o.handle_;
  }
  void swap(openssl_bio_native_handle& o) noexcept {
    o.handle_.swap(this->handle_);
  }
  openssl_bio_native_handle& operator=(
    openssl_bio_native_handle const& rhs
  ) noexcept {
    openssl_bio_native_handle tmp(rhs);
    tmp.swap(*this);
    return *this;
  }

  openssl_bio_native_handle(
    openssl_bio_native_handle&& rhs
  ) noexcept
    : openssl_bio_handle(std::move(rhs)) {}
  openssl_bio_native_handle& operator=(
    openssl_bio_native_handle&& rhs
  ) noexcept {
    if (this == &rhs) {
      return *this;
    }

    openssl_bio_handle::operator=(std::move(rhs));
    return *this;
  }

  [[nodiscard]]
  T native_handle() const {
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

  [[nodiscard]]
  openssl_bio_native_handle up_ref() const {
    if (BIO_up_ref(this->to_raw()) != 1) {
      throw std::runtime_error("up_ref failed");
    }
    return openssl_bio_native_handle(*this);
  }
};

[[nodiscard]]
std::string get_openssl_error_messages() noexcept;

[[nodiscard]]
std::filesystem::path const& get_certs_dir() noexcept;

}

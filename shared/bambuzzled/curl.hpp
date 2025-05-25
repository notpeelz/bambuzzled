#pragma once

#include <string>
#include <memory>
#include <curl/curl.h>

namespace bbzl::utils::curl {

struct curl_handle_deleter {
  void operator()(CURL* curl) const {
    if (curl != nullptr) {
      curl_easy_cleanup(curl);
    }
  }
};

using curl_handle_base = std::unique_ptr<CURL, curl_handle_deleter>;
class curl_handle
  : public curl_handle_base
{
public:
  using curl_handle_base::curl_handle_base;

  bool is_valid() const noexcept {
    return this->get() != nullptr;
  }

  operator CURL*() const noexcept {
    return this->get();
  }
};

std::string urlencode(std::string const& s);

std::string urldecode(std::string const& s);

}

#pragma once

#include <memory>

#include <curl/curl.h>

namespace bbzl::curl {

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
  operator CURL*() const { return this->get(); }
};

}

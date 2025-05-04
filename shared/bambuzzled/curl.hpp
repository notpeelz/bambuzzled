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

class curl_handle
  : public std::unique_ptr<CURL, curl_handle_deleter>
{
public:
  using base = std::unique_ptr<CURL, curl_handle_deleter>;
  using base::base;
  operator CURL*() const { return this->get(); }
};

}

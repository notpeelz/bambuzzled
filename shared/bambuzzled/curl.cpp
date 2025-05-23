#include <string>
#include <stdexcept>
#include <curl/curl.h>
#include "curl.hpp"

namespace bbzl::curl {

std::string urlencode(std::string const& s) {
  char* encoded = curl_easy_escape(
    nullptr,
    s.c_str(),
    static_cast<int>(s.length())
  );
  if (encoded == nullptr) {
    throw std::runtime_error("failed to urlencode string");
  }

  auto result = std::string(encoded);
  curl_free(encoded);
  return result;
}

std::string urldecode(std::string const& s) {
  int length = 0;
  char* decoded = curl_easy_unescape(
    nullptr,
    s.c_str(),
    static_cast<int>(s.length()),
    &length
  );
  if (decoded == nullptr) {
    throw std::runtime_error("failed to urldecode string");
  }

  auto result = std::string(decoded, length);
  curl_free(decoded);
  return result;
}

}

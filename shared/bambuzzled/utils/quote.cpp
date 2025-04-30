#include <string>
#include <sstream>

#include "quote.hpp"

namespace bbzl::utils {

static void replace_all_mut(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
}

std::string quote_string(std::string_view s) {
  return quote_string(std::string(s));
}

std::string quote_string(std::string s) {
  std::ostringstream oss;
  replace_all_mut(s, "\"", "\\\"");
  oss << "\"" << s << "\"";
  return oss.str();
}

}

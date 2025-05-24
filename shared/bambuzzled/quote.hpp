#pragma once

#include <string>
#include <string_view>

namespace bbzl::utils {

[[nodiscard]]
std::string quote_string(std::string_view s);

[[nodiscard]]
std::string quote_string(std::string s);

}

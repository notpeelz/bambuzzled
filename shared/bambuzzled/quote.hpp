#pragma once

#include <string>
#include <string_view>

namespace bbzl::utils {

std::string quote_string(std::string_view s);
std::string quote_string(std::string s);

}

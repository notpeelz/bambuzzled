#pragma once

#include <optional>
#include <functional>

namespace bbzl::utils {

template<typename T>
using optional_ref = std::optional<std::reference_wrapper<T>>;

}

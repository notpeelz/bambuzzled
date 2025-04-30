#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <type_traits>
#include <typeinfo>

#include "demangle.hpp"
#include "quote.hpp"

namespace bbzl::utils {

template<typename T>
static inline std::string stringify_value(const T& value) {
  std::ostringstream oss;
  if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T>) {
    oss << value;
  } else if constexpr (
    std::is_convertible_v<T, std::string>
    || std::is_convertible_v<T, std::string_view>
  ) {
    oss << quote_string(value);
  } else {
    oss << "[Object of type " << demangle(typeid(value).name()) << " at " << &value << "]";
  }
  return oss.str();
}

static inline std::string stringify_value(const std::string& value) {
  return quote_string(value);
}

static inline std::string stringify_value(const std::string_view& value) {
  return quote_string(value);
}

static inline std::string stringify_value(std::nullptr_t) {
  return "nullptr";
}

template<typename T>
std::string stringify_pointer(const T* ptr) {
  if (!ptr) {
    return "nullptr";
  }

  std::ostringstream oss;
  oss << "ptr(" << demangle(typeid(T).name()) << ")[" << (void*)ptr << "]";

  if constexpr (std::is_same_v<T, char>) {
    oss << "→" << stringify_value(std::string_view(ptr));
  } else if constexpr (
    std::is_arithmetic_v<T>
    || std::is_convertible_v<T, std::string>
    || std::is_convertible_v<T, std::string_view>
    || std::is_enum_v<T>
  ) {
    oss << "→" << stringify_value(*ptr);
  }

  return oss.str();
}

static inline std::string stringify_void_pointer(const void* ptr) {
  if (!ptr) {
    return "nullptr";
  }

  std::ostringstream oss;
  oss << "void*[" << ptr << "]";
  return oss.str();
}

template<typename T>
static inline std::string to_string(const T& value) {
  return stringify_value(value);
}

template<typename T>
std::string to_string(T* ptr) {
  return stringify_pointer(ptr);
}

static inline std::string to_string(void* ptr) {
  return stringify_void_pointer(ptr);
}

static inline std::string to_string(const void* ptr) {
  return stringify_void_pointer(ptr);
}

}

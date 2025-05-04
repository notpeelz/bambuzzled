#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <type_traits>
#include <typeinfo>

#include "demangle.hpp"
#include "quote.hpp"

namespace {

template<typename, typename = void>
constexpr bool is_type_complete_v = false;

template<typename T>
constexpr bool is_type_complete_v<
  T,
  std::void_t<decltype(sizeof(T))>
> = true;

}

namespace bbzl::utils {

template<typename T>
static inline std::string stringify_value(T const& value) {
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

static inline std::string stringify_value(std::string const& value) {
  return quote_string(value);
}

static inline std::string stringify_value(std::string_view value) {
  return quote_string(value);
}

static inline std::string stringify_value(std::nullptr_t) {
  return "nullptr";
}

static inline std::string stringify_void_pointer(void const* ptr) {
  if (ptr == nullptr) {
    return "nullptr";
  }

  std::ostringstream oss;
  oss << "void*[" << ptr << "]";
  return oss.str();
}

template<typename T>
std::string stringify_pointer(T const* ptr) {
  if (ptr == nullptr) {
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

template<typename T>
static inline std::string to_string(T const& value) {
  return stringify_value(value);
}

template<typename T>
std::string to_string(T* ptr) {
  if constexpr (!is_type_complete_v<T>) {
    return stringify_void_pointer(static_cast<void const*>(ptr));
  } else {
    return stringify_pointer(ptr);
  }
}

static inline std::string to_string(void* ptr) {
  return stringify_void_pointer(ptr);
}

static inline std::string to_string(void const* ptr) {
  return stringify_void_pointer(ptr);
}

}

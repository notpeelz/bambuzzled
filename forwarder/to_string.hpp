#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include <typeinfo>
#include <memory>

#if defined (_WIN32) && defined(_MSC_VER)

#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

std::string demangle(const char* name) {
  if (name[0] != '?') {
    return name;
  }

  char undecorated_name[1024];
  if (UnDecorateSymbolName(
    name,
    undecorated_name,
    sizeof(undecorated_name),
    UNDNAME_COMPLETE
  )) {
    return undecorated_name;
  }

  return name;
}

#else

#include <cxxabi.h>

std::string demangle(const char* name) {
  int status = 0;
  std::unique_ptr<char, void(*)(void*)> res {
    abi::__cxa_demangle(name, nullptr, nullptr, &status),
    std::free
  };
  return status == 0 ? res.get() : name;
}

#endif

void replace_all_mut(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
}

std::string quote_string(std::string s) {
  std::ostringstream oss;
  replace_all_mut(s, "\"", "\\\"");
  oss << "\"" << s << "\"";
  return oss.str();
}

template<typename T>
std::string stringify_value(const T& value) {
  std::ostringstream oss;
  if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T>) {
    oss << value;
  } else if constexpr (std::is_convertible_v<T, std::string>) {
    oss << quote_string(value);
  } else {
    oss << "[Object of type " << demangle(typeid(value).name()) << " at " << &value << "]";
  }
  return oss.str();
}

std::string stringify_value(const std::string& value) {
  return quote_string(value);
}

std::string stringify_value(std::nullptr_t) {
  return "nullptr";
}

template<typename T>
std::string stringify_pointer(const T* ptr) {
  if (!ptr) {
    return "nullptr";
  }

  std::ostringstream oss;
  oss << "ptr(" << demangle(typeid(T).name()) << ")[" << ptr << "]";

  if constexpr (
    std::is_arithmetic_v<T>
    || std::is_convertible_v<T, std::string>
    || std::is_enum_v<T>
  ) {
    oss << "→" << stringify_value(*ptr);
  }

  return oss.str();
}

std::string stringify_void_pointer(const void* ptr) {
  if (!ptr) {
    return "nullptr";
  }

  std::ostringstream oss;
  oss << "void*[" << ptr << "]";
  return oss.str();
}

template<typename T>
std::string to_string(const T& value) {
  return stringify_value(value);
}

template<typename T>
std::string to_string(T* ptr) {
  return stringify_pointer(ptr);
}

std::string to_string(void* ptr) {
  return stringify_void_pointer(ptr);
}

std::string to_string(const void* ptr) {
  return stringify_void_pointer(ptr);
}

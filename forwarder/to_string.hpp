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
}

#else

#include <cxxabi.h>

// Helper to get demangled type name
std::string demangle(const char* name) {
  int status = 0;
  std::unique_ptr<char, void(*)(void*)> res {
    abi::__cxa_demangle(name, nullptr, nullptr, &status),
    std::free
  };
  return status == 0 ? res.get() : name;
}

#endif

// Forward declarations
template<typename T>
std::string stringify_value(const T& value);

template<typename T>
std::string stringify_pointer(const T* ptr);

std::string stringify_void_pointer(const void* ptr);

// Base stringification function - tests for streamability
template<typename T>
std::string stringify_value(const T& value) {
  // Try using operator<< if available
  if constexpr (std::is_convertible_v<T, std::string> || 
    std::is_arithmetic_v<T> ||
    std::is_enum_v<T>) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
  } else {
    // Fallback for types without stream operators
    std::ostringstream oss;
    oss << "[Object of type " << demangle(typeid(value).name()) << " at " << &value << "]";
    return oss.str();
  }
}

// Special case for strings
std::string stringify_value(const std::string& value) {
  return value;
}

// Special case for nullptr
std::string stringify_value(std::nullptr_t) {
  return "nullptr";
}

// Pointer stringification
template<typename T>
std::string stringify_pointer(const T* ptr) {
  if (!ptr) return "nullptr";

  std::ostringstream oss;
  oss << "ptr(" << demangle(typeid(T).name()) << ")[" << ptr << "]";

  // For simple types, try to show their value
  if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_same_v<T, std::string>) {
    oss << "→" << stringify_value(*ptr);
  }

  return oss.str();
}

// Special case for void pointers
std::string stringify_void_pointer(const void* ptr) {
  if (!ptr) return "nullptr";

  std::ostringstream oss;
  oss << "void*[" << ptr << "]";
  return oss.str();
}

// Universal to_string that dispatches to the right function
template<typename T>
std::string to_string(const T& value) {
  return stringify_value(value);
}

// Overload for regular pointers
template<typename T>
std::string to_string(T* ptr) {
  return stringify_pointer(ptr);
}

// Overload for void pointers
std::string to_string(void* ptr) {
  return stringify_void_pointer(ptr);
}

// Overload for const void pointers
std::string to_string(const void* ptr) {
  return stringify_void_pointer(ptr);
}
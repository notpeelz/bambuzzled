#pragma once

#include <string>
#include <sstream>
#include <format>
#include <source_location>

#include "to_string.hpp"

namespace bbzl::log {

enum class log_level {
  TRACE,
  DEBUG,
  INFO,
  WARN,
  ERROR,
};

void log_internal(log_level level, const std::string& msg) noexcept;

template<typename... Args>
static inline void trace_fmt(std::format_string<Args...> fmt, Args&&... args) noexcept {
  log_internal(log_level::TRACE, std::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
static inline void debug_fmt(std::format_string<Args...> fmt, Args&&... args) noexcept {
  log_internal(log_level::DEBUG, std::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
static inline void info_fmt(std::format_string<Args...> fmt, Args&&... args) noexcept {
  log_internal(log_level::INFO, std::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
static inline void warn_fmt(std::format_string<Args...> fmt, Args&&... args) noexcept {
  log_internal(log_level::WARN, std::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
static inline void error_fmt(std::format_string<Args...> fmt, Args&&... args) noexcept {
  log_internal(log_level::ERROR, std::format(fmt, std::forward<Args>(args)...));
}

static inline void trace(const std::string& s) noexcept {
  log_internal(log_level::TRACE, s);
}

static inline void debug(const std::string& s) noexcept {
  log_internal(log_level::DEBUG, s);
}

static inline void info(const std::string& s) noexcept {
  log_internal(log_level::INFO, s);
}

static inline void warn(const std::string& s) noexcept {
  log_internal(log_level::WARN, s);
}

static inline void error(const std::string& s) noexcept {
  log_internal(log_level::ERROR, s);
}

class trace_scope_guard {
private:
  const std::source_location location;

public:
  trace_scope_guard(const std::source_location location)
    : location(location) {}

  ~trace_scope_guard() {
    trace_fmt("EXIT: {}", this->location.function_name());
  }
};

template<typename... Args>
static inline trace_scope_guard trace_scope(
  const std::source_location&& location,
  Args&&... args
) {
  std::ostringstream oss;
  oss << "ENTER: " << location.function_name();

  size_t i = 0;
  (([&](const auto& arg) {
    oss << "\n  " << i << ": " << bbzl::utils::to_string(arg);
    i++;
  })(args), ...);

  trace(oss.str());
  return trace_scope_guard(std::move(location));
}

#define BBZL_TRACE_SCOPE(...) \
::bbzl::log::trace_scope( \
  std::source_location::current() \
  __VA_OPT__(,) __VA_ARGS__ \
)

}

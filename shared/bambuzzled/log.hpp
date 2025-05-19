#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
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

void set_level_filter(log_level level) noexcept;

void log_internal(log_level level, std::string const& msg) noexcept;

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

static inline void trace(std::string const& s) noexcept {
  log_internal(log_level::TRACE, s);
}

static inline void debug(std::string const& s) noexcept {
  log_internal(log_level::DEBUG, s);
}

static inline void info(std::string const& s) noexcept {
  log_internal(log_level::INFO, s);
}

static inline void warn(std::string const& s) noexcept {
  log_internal(log_level::WARN, s);
}

static inline void error(std::string const& s) noexcept {
  log_internal(log_level::ERROR, s);
}

class trace_scope_guard {
private:
  std::string name;

public:
  trace_scope_guard(std::string name) noexcept
    : name(std::move(name)) {}

  ~trace_scope_guard() noexcept {
    trace_fmt("EXIT: {}", this->name);
  }
};

template<typename... Args>
static inline trace_scope_guard trace_scope(
  std::source_location const&& location,
  Args&&... args
) noexcept {
  std::ostringstream oss;
  oss << "ENTER: " << location.function_name();

  size_t i = 0;
  (([&](auto const& arg) {
    oss << "\n  " << i << ": " << bbzl::utils::to_string(arg);
    i++;
  })(args), ...);

  trace(oss.str());
  return trace_scope_guard(location.function_name());
}

#define BBZL_TRACE_SCOPE(...) \
::bbzl::log::trace_scope( \
  std::source_location::current() \
  __VA_OPT__(,) __VA_ARGS__ \
)

template<typename... Args>
static inline trace_scope_guard trace_scope_named(
  std::string_view name,
  std::source_location const&& location,
  Args&&... args
) noexcept {
  std::ostringstream oss;
  oss << "ENTER: " << name << ": " << location.function_name();

  size_t i = 0;
  (([&](auto const& arg) {
    oss << "\n  " << i << ": " << bbzl::utils::to_string(arg);
    i++;
  })(args), ...);

  trace(oss.str());
  return trace_scope_guard(location.function_name());
}

#define BBZL_TRACE_SCOPE_NAMED(name, ...) \
::bbzl::log::trace_scope_named( \
  name, \
  std::source_location::current() \
  __VA_OPT__(,) __VA_ARGS__ \
)

}

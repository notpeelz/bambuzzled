#pragma once

#include <string>
#include <format>

namespace bambuzzled::log {

  enum class log_level {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
  };

  void log_internal(log_level level, const std::string& msg) noexcept;

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

  template<typename... Args>
  static inline void trace(std::format_string<Args...> fmt, Args&&... args) noexcept {
    log_internal(log_level::TRACE, std::format(fmt, std::forward<Args>(args)...));
  }

  template<typename... Args>
  static inline void debug(std::format_string<Args...> fmt, Args&&... args) noexcept {
    log_internal(log_level::DEBUG, std::format(fmt, std::forward<Args>(args)...));
  }

  template<typename... Args>
  static inline void info(std::format_string<Args...> fmt, Args&&... args) noexcept {
    log_internal(log_level::INFO, std::format(fmt, std::forward<Args>(args)...));
  }

  template<typename... Args>
  static inline void warn(std::format_string<Args...> fmt, Args&&... args) noexcept {
    log_internal(log_level::WARN, std::format(fmt, std::forward<Args>(args)...));
  }

  template<typename... Args>
  static inline void error(std::format_string<Args...> fmt, Args&&... args) noexcept {
    log_internal(log_level::ERROR, std::format(fmt, std::forward<Args>(args)...));
  }

}

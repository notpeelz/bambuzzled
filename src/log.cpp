#include <algorithm>
#include <optional>
#include <iostream>
#include <fstream>
#include <mutex>
#include <string_view>
#include <format>

#include "log.hpp"

using namespace std::string_view_literals;

namespace bbzl::log {

  static std::string_view level_to_str(log_level level) noexcept {
    switch (level) {
      case log_level::TRACE: {
        return "TRACE";
      }
      case log_level::DEBUG: {
        return "DEBUG";
      }
      case log_level::INFO: {
        return "INFO ";
      }
      case log_level::WARN: {
        return "WARN ";
      }
      case log_level::ERROR: {
        return "ERROR";
      }
      default: {
        return "UNKNOWN";
      }
    }
  }

  static std::optional<log_level> str_to_level(const char* level) noexcept {
    if (level == nullptr) {
      return std::nullopt;
    }

    auto level_normalized = std::string(level);
    std::transform(
      level_normalized.begin(),
      level_normalized.end(),
      level_normalized.begin(),
      [](unsigned char c) {
        return std::tolower(c);
      }
    );

    if (level_normalized == "trace") {
      return log_level::TRACE;
    }

    if (level_normalized == "debug") {
      return log_level::DEBUG;
    }

    if (level_normalized == "info") {
      return log_level::INFO;
    }

    if (level_normalized == "warn") {
      return log_level::WARN;
    }

    if (level_normalized == "error") {
      return log_level::ERROR;
    }

    return std::nullopt;
  }

  static bool initialized = false;
  static log_level current_level;
  static std::mutex mutex;
  static std::ofstream log_file;

  static void ensure_initialized() noexcept {
    if (initialized) {
      return;
    }
    initialized = true;

    char* level_str = std::getenv("BBZL_LOG_LEVEL");
    auto level = str_to_level(level_str);
    if (level.has_value()) {
      current_level = level.value();
    } else {
      current_level = log_level::INFO;
    }

    char* log_file_str = std::getenv("BBZL_LOG_FILE");
    if (log_file_str != nullptr) {
      try {
        log_file.open(log_file_str, std::ios::out);
      } catch (...) {}
    }
  }

  static std::string current_time_str() noexcept {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;

#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    char buffer[32];
    (void)std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    return buffer;
  }

  void log_internal(log_level level, const std::string& msg) noexcept {
    if (level < current_level) {
      return;
    }

    std::lock_guard<std::mutex> lock(mutex);
    ensure_initialized();

    auto log_line = std::format(
      "[{}] [{}] {}\n"sv,
      current_time_str(),
      level_to_str(level),
      msg
    );

    std::cout << log_line << std::flush;

    if (log_file.is_open()) {
      log_file << log_line;
      log_file.flush();
    }
  }
}

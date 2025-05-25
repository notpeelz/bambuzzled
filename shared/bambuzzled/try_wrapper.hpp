#pragma once

#include <utility>
#include <concepts>
#include <exception>
#include <type_traits>
#include <source_location>
#include "log.hpp"

namespace {

static inline void log_exception(
  std::exception const& ex,
  std::source_location const& location
) noexcept {
  bbzl::log::error_fmt(
    "CAUGHT EXCEPTION: {}\n"
    "  Function: {}\n"
    "  Location: {}:{}",
    ex.what(),
    location.function_name(),
    location.file_name(),
    location.line()
  );
}

}

namespace bbzl::utils::try_wrapper {

template<typename F>
static inline void try_(
  F fn,
  std::source_location const&& location = std::source_location::current()
) noexcept requires std::invocable<F> {
  try {
    fn();
  } catch (std::exception& ex) {
    log_exception(ex, location);
  }
}

template<typename F>
static inline auto try_or_default(
  F fn,
  std::source_location const&& location = std::source_location::current()
) noexcept -> typename std::invoke_result_t<F>
requires std::invocable<F> {
  try {
    return fn();
  } catch (std::exception& ex) {
    log_exception(ex, location);
    return typename std::invoke_result_t<F>{};
  }
}

template<typename F>
static inline auto try_or(
  typename std::invoke_result_t<F> fallback,
  F fn,
  std::source_location const&& location = std::source_location::current()
) noexcept -> typename std::invoke_result_t<F>
requires std::invocable<F> {
  try {
    return fn();
  } catch (std::exception& ex) {
    log_exception(ex, location);
    return fallback;
  }
}

template<typename F>
static auto wrap_try(
  F fn,
  std::source_location const&& location = std::source_location::current()
) noexcept {
  return [
    fn = std::move(fn),
    location
  ](auto&&... args) {
    try {
      fn(std::forward<decltype(args)>(args)...);
    } catch (std::exception const& ex) {
      log_exception(ex, location);
    }
  };
}

}

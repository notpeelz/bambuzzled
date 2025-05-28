#pragma once

#include <string>
#include <format>
#include <optional>
#include <stdexcept>
#include <functional>
#include <bambuzzled/socket.hpp>
#include <bambuzzled/optional_ref.hpp>

#if defined(_WIN32)
#include <windows.h>
#else
#include <netinet/in.h>
#endif

namespace bbzl::pairing {

class probe_binding_error
  : public std::runtime_error
{
private:
  std::string reason_;

public:
  probe_binding_error(
    std::string const& msg,
    std::string const& reason
  )
    : std::runtime_error(std::format("{}: {}", msg, reason)),
      reason_(reason) {}

  probe_binding_error(std::string const& msg)
    : std::runtime_error(msg) {}

  [[nodiscard]]
  bbzl::utils::optional_ref<std::string const> reason() {
    return std::make_optional(std::cref(this->reason_));
  }
};

class malformed_response
  : public probe_binding_error
{
private:
  static constexpr char const msg[] =
    "invalid binding probe response";

public:
  malformed_response(std::string const& reason)
    : probe_binding_error(msg, reason) {}

  malformed_response()
    : probe_binding_error(msg) {}
};

[[nodiscard]]
std::string probe_binding(
  in_addr addr,
  bbzl::utils::net::net_port port,
  bool use_ssl
);

[[nodiscard]]
std::string probe_binding(
  in6_addr addr,
  bbzl::utils::net::net_port port,
  bool use_ssl
);

}

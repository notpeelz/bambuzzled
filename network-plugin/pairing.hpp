#pragma once

#include <string>
#include <bambuzzled/socket.hpp>

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/socket.h>
#endif

namespace bbzl::pairing {

[[nodiscard]]
std::string probe_binding_info(
  in_addr addr,
  bbzl::utils::net::net_port port,
  bool use_ssl
);

[[nodiscard]]
std::string probe_binding_info(
  in6_addr addr,
  bbzl::utils::net::net_port port,
  bool use_ssl
);

}

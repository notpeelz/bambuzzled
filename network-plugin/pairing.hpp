#pragma once

#include <bambuzzled/networking/api/bambu_networking.hpp>
#include <string>

namespace bbzl::pairing {

void discover(
  std::string const& addr,
  bool use_ssl,
  BBL::detectResult& result
);

}

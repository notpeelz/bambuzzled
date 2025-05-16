#pragma once

#include <string>

#include <bambuzzled/networking/api/bambu_networking.hpp>

namespace bbzl::pairing {

void discover(
  std::string const& addr,
  bool use_ssl,
  BBL::detectResult& result
);

}

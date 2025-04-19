#pragma once

#include <string>

#include <bambuzzled/api/bambu_networking.hpp>

class network_agent {
private:
  std::string config_dir;
public:
  network_agent();
  void set_config_dir(std::string dir);
};

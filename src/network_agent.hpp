#pragma once

#include <string>
#include <memory>

#include <bambuzzled/api/bambu_networking.hpp>

#include "mosquitto.hpp"

class network_agent {
private:
  std::string dev_id;
  std::string config_dir;
  mosquitto mosq;

  friend void on_connect(
    struct mosquitto* mosq,
    void* obj,
    int reason_code
  );
  friend void on_disconnect(
    struct mosquitto* mosq,
    void* obj,
    int reason_code
  );
  friend void on_message(
    struct mosquitto* mosq,
    void* obj,
    const struct mosquitto_message* message
  );
public:
  network_agent();
  void set_config_dir(std::string dir);
  int start();
  int connect_printer(
    std::string dev_id,
    std::string dev_ip,
    std::string username,
    std::string password,
    bool use_ssl
  );
};

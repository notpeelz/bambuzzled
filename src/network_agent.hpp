#pragma once

#include <string>
#include <memory>

#include <bambuzzled/api/bambu_networking.hpp>

#include "mosquittopp.hpp"

class network_agent;

struct network_client {
  network_agent* agent;
  std::string dev_id;
  mosquittopp* mosq;
  ~network_client() {
    if (this->mosq != nullptr) {
      delete this->mosq;
    }
  }
};

class network_agent {
private:
  std::string config_dir;
  std::map<std::string, network_client*> connections;
  OnPrinterConnectedFn on_printer_connected_fn;
  OnServerConnectedFn on_server_connected_fn;
  OnLocalConnectedFn on_local_connected_fn;
  OnMessageFn on_local_message_fn;

  friend void on_connect(
    mosquitto* mosq,
    void* obj,
    int reason_code
  );
  friend void on_disconnect(
    mosquitto* mosq,
    void* obj,
    int reason_code
  );
  friend void on_message(
    mosquitto* mosq,
    void* obj,
    const mosquitto_message* message
  );
  network_client* get_or_create_conn(std::string& dev_id);
public:
  network_agent();
  void set_config_dir(std::string&& dir);
  void set_on_printer_connected_fn(OnPrinterConnectedFn&& fn);
  void set_on_server_connected_fn(OnServerConnectedFn&& fn);
  void set_on_local_connected_fn(OnLocalConnectedFn&& fn);
  void set_on_local_message_fn(OnMessageFn&& fn);
  std::string get_user_selected_machine();
  void set_user_selected_machine(std::string dev_id);
  int start();
  int connect_printer(
    std::string& dev_id,
    std::string& dev_ip,
    std::string& username,
    std::string& password,
    bool use_ssl
  );
  int send_message_to_printer(
    std::string& dev_id,
    std::string& json_str,
    int qos
  );
};

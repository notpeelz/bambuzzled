#pragma once

#include <string>
#include <memory>

#include <bambuzzled/api/bambu_networking.hpp>

#include "mosquittopp.hpp"

namespace bambuzzled {

class network_agent;

class network_client {
  friend network_agent;
private:
  void on_connect(int reason_code);
  void on_disconnect(int reason_code);
  void on_message(const mosquitto_message* msg);
public:
  network_agent* agent;
  std::string dev_id;
  mqtt::mosquittopp* mosqpp;
  ~network_client() {
    if (this->mosqpp != nullptr) {
      delete this->mosqpp;
    }
  }
};

class network_agent {
  friend network_client;
private:
  std::string config_dir;
  std::map<std::string, network_client*> connections;
  OnPrinterConnectedFn on_printer_connected_fn;
  OnServerConnectedFn on_server_connected_fn;
  OnLocalConnectedFn on_local_connected_fn;
  OnMessageFn on_local_message_fn;
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

}

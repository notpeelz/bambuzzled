#include <iostream>
#include <mutex>
#include <stdexcept>
#include <cassert>

#include "log.hpp"
#include "mosquittopp.hpp"
#include "network_agent.hpp"

namespace bbzl {

using namespace std::string_view_literals;

network_agent::network_agent() {}

void network_agent::set_config_dir(std::string&& dir) {
  log::trace_fmt("set_config_dir: {}", dir);
}

void network_agent::set_on_printer_connected_fn(OnPrinterConnectedFn&& fn) {
  this->on_printer_connected_fn = std::move(fn);
}

void network_agent::set_on_server_connected_fn(OnServerConnectedFn&& fn) {
  this->on_server_connected_fn = std::move(fn);
}

void network_agent::set_on_local_connected_fn(OnLocalConnectedFn&& fn) {
  this->on_local_connected_fn = std::move(fn);
}

void network_agent::set_on_local_message_fn(OnMessageFn&& fn) {
  this->on_local_message_fn = std::move(fn);
}

std::string network_agent::get_user_selected_machine() {
  return "";
}

void network_agent::set_user_selected_machine(std::string dev_id) {
  (void)dev_id;
}

int network_agent::start() {
  auto scope = BBZL_TRACE_SCOPE();
  return 0;
}

void network_client::on_connect(int reason_code) {
  auto scope = BBZL_TRACE_SCOPE(reason_code);
  this->mosqpp->subscribe("device/" + this->dev_id + "/report", 1);

  // TODO(?): return_code = 5: "login information expired"
  this->agent->on_server_connected_fn(0, reason_code);

  if (reason_code != 0) {
    log::error_fmt(
      "failed connecting to MQTT broker: {}",
      mosquitto_connack_string(reason_code)
    );
  } else {
    log::info("connected to MQTT broker");
  }
}

void network_client::on_disconnect(
  int reason_code
) {
  log::info("disconnected from MQTT broker");
  if (reason_code != 0) {
    log::error_fmt("unexpected disconnect: {}", mosquitto_strerror(reason_code));
  }
}

void network_client::on_message(
  const mosquitto_message* msg
) {
  auto msg_str = std::string((char*)msg->payload, msg->payloadlen);
  log::trace_fmt("received message ({}): {}", msg->topic, msg_str);
  this->agent->on_local_message_fn(this->dev_id, msg_str);
}

network_client* network_agent::get_or_create_conn(std::string& dev_id) {
  auto pos = this->connections.find(dev_id);
  if (pos != this->connections.end()) {
    return pos->second;
  }

  auto conn = new network_client{
    .agent = this,
    .dev_id = dev_id,
    .mosqpp = new mqtt::mosquittopp(),
  };

  conn->mosqpp->set_on_connect_cb([conn](mqtt::mosquittopp&, int reason_code) {
    conn->on_connect(reason_code);
  });
  conn->mosqpp->set_on_disconnect_cb([conn](mqtt::mosquittopp&, int reason_code) {
    conn->on_disconnect(reason_code);
  });
  conn->mosqpp->set_on_message_cb([conn](mqtt::mosquittopp&, const mosquitto_message* msg) {
    conn->on_message(msg);
  });

  const auto& [_, emplaced] = this->connections.emplace(dev_id, conn);
  assert(emplaced);
  return conn;
}

int network_agent::connect_printer(
  std::string& dev_id,
  std::string& dev_ip,
  std::string& username,
  std::string& password,
  bool use_ssl
) {
  (void)use_ssl; // TODO?
  auto scope = BBZL_TRACE_SCOPE(dev_id, dev_ip, username, "*PASSWORD*"sv);
  auto conn = this->get_or_create_conn(dev_id);
  conn->mosqpp->connect(dev_ip, 8883, username, password, 60);
  return 0;
}

int network_agent::send_message_to_printer(
  std::string& dev_id,
  std::string& json_str,
  int qos
) {
  auto scope = BBZL_TRACE_SCOPE(dev_id, json_str, qos);
  auto conn = this->get_or_create_conn(dev_id);
  conn->mosqpp->publish("device/" + dev_id + "/request", json_str, qos);
  return 0;
}

}

#include <iostream>
#include <mutex>
#include <stdexcept>

#include "mosquittopp.hpp"
#include "network_agent.hpp"

network_agent::network_agent() {}

void network_agent::set_config_dir(std::string&& dir) {
  std::cerr << "set_config_dir: " << dir << std::endl;
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

int network_agent::start() {
  std::cerr << "start" << std::endl;
  return 0;
}

void on_connect(
  mosquitto*,
  void* obj,
  int reason_code
) {
  auto conn = reinterpret_cast<mosquittopp_connection*>(obj);

  std::string topic = "device/" + conn->dev_id + "/report";
  int qos = 1; // QoS level 1 (at least once)

  int rc = mosquitto_subscribe(conn->mosq->get(), nullptr, topic.c_str(), qos);
  if (rc != MOSQ_ERR_SUCCESS) {
    std::cerr << "Error subscribing to topic: " << mosquitto_strerror(rc) << std::endl;
  } else {
    std::cout << "Subscribed to topic: " << topic << std::endl;
  }

  // return_code = 5: "login information expired"
  conn->agent->on_server_connected_fn(0, reason_code);

  // if (reason_code != 0) {
  //   std::cerr << "Failed connecting to broker: " << mosquitto_connack_string(reason_code) << std::endl;
  // } else {
  //   std::cout << "Connected to MQTT broker successfully" << std::endl;
  // }
}

void on_disconnect(
  mosquitto*,
  void* obj,
  int reason_code
) {
  (void)obj;
  std::cout << "Disconnected from broker" << std::endl;
  if (reason_code != 0) {
    std::cerr << "Unexpected disconnect: " << mosquitto_strerror(reason_code) << std::endl;
  }
}

void on_message(
  mosquitto*,
  void* obj,
  const mosquitto_message* msg
) {
  auto conn = reinterpret_cast<mosquittopp_connection*>(obj);
  auto msg_str = std::string((char*)msg->payload, msg->payloadlen);
  std::cout << "Received message (" << msg->topic << "): " << msg_str << std::endl;
  conn->agent->on_local_message_fn(conn->dev_id, msg_str);
}

mosquittopp_connection* network_agent::get_or_create_conn(std::string& dev_id) {
  auto conn = new mosquittopp_connection{
    .agent = this,
    .dev_id = dev_id,
    .mosq = nullptr,
  };
  conn->mosq = new mosquittopp(conn);
  const auto& [pair, emplaced] = this->connections.try_emplace(dev_id, conn);

  if (emplaced) {
    conn->mosq->start();
    return conn;
  }

  return pair->second;
}

int network_agent::connect_printer(
  std::string& dev_id,
  std::string& dev_ip,
  std::string& username,
  std::string& password,
  bool use_ssl
) {
  (void)use_ssl; // TODO?
  int rc;

  std::cout << "connect_printer" << std::endl;
  auto conn = this->get_or_create_conn(dev_id);

  rc = mosquitto_username_pw_set(conn->mosq->get(), username.c_str(), password.c_str());
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error("failed to set MQTT credentials");
  }

  mosquitto_connect_callback_set(conn->mosq->get(), on_connect);
  mosquitto_message_callback_set(conn->mosq->get(), on_message);
  mosquitto_disconnect_callback_set(conn->mosq->get(), on_disconnect);

  rc = mosquitto_connect(conn->mosq->get(), dev_ip.c_str(), 8883, 60);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error("failed to connect to MQTT broker: " + std::string(mosquitto_strerror(rc)));
  }

  return 0;
}

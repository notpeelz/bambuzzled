#include <iostream>
#include <mutex>
#include <stdexcept>

#include "mosquitto.hpp"
#include "network_agent.hpp"

network_agent::network_agent() : mosq(this) {}

void network_agent::set_config_dir(std::string dir) {
  std::cerr << "set_config_dir: " << dir << std::endl;
}

int network_agent::start() {
  std::cerr << "start" << std::endl;
  return 0;
}

void on_connect(
  struct mosquitto* mosq,
  void* obj,
  int reason_code
) {
  auto agent = reinterpret_cast<network_agent*>(obj);
  if (reason_code != 0) {
    std::cerr << "Failed connecting to broker: " << mosquitto_connack_string(reason_code) << std::endl;
    return;
  }

  std::cout << "Connected to MQTT broker successfully" << std::endl;

  std::string topic = "device/" + agent->dev_id + "/report";
  int qos = 1; // QoS level 1 (at least once)

  int rc = mosquitto_subscribe(mosq, nullptr, topic.c_str(), qos);
  if (rc != MOSQ_ERR_SUCCESS) {
    std::cerr << "Error subscribing to topic: " << mosquitto_strerror(rc) << std::endl;
  } else {
    std::cout << "Subscribed to topic: " << topic << std::endl;
  }
}

void on_disconnect(
  struct mosquitto* mosq,
  void* obj,
  int reason_code
) {
  (void)mosq;
  (void)obj;
  std::cout << "Disconnected from broker" << std::endl;
  if (reason_code != 0) {
    std::cerr << "Unexpected disconnect: " << mosquitto_strerror(reason_code) << std::endl;
  }
}

void on_message(
  struct mosquitto* mosq,
  void* obj,
  const struct mosquitto_message* message
) {
  (void)mosq;
  (void)obj;
  std::cout << "Received message on topic: " << message->topic << std::endl;
  std::cout << "Message: " << std::string((char*)message->payload, message->payloadlen) << std::endl;
}

int network_agent::connect_printer(
  std::string dev_id,
  std::string dev_ip,
  std::string username,
  std::string password,
  bool use_ssl
) {
  (void)use_ssl; // TODO?
  int rc;

  this->dev_id = dev_id;

  rc = mosquitto_username_pw_set(this->mosq.get(), username.c_str(), password.c_str());
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error("failed to set MQTT credentials");
  }

  mosquitto_connect_callback_set(this->mosq.get(), on_connect);
  mosquitto_message_callback_set(this->mosq.get(), on_message);
  mosquitto_disconnect_callback_set(this->mosq.get(), on_disconnect);

  rc = mosquitto_connect(this->mosq.get(), dev_ip.c_str(), 8883, 60);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error("failed to connect to MQTT broker: " + std::string(mosquitto_strerror(rc)));
  }

  this->mosq.start();

  return 0;
}

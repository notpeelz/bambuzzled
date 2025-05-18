#include <cassert>
#include <format>
#include <span>
#include <chrono>
#include <thread>
#include <utility>
#include <string>
#include <string_view>
#include <optional>
#include <stdexcept>
#include <mosquitto.h>
#include <bambuzzled/log.hpp>
#include <bambuzzled/try_wrapper.hpp>
#include <bambuzzled/networking/api/bambu_networking.hpp>
#include "mosquittopp.hpp"
#include "network_agent.hpp"

namespace bbzl {

using namespace std::chrono_literals;
using namespace std::string_view_literals;
using namespace bbzl::try_wrapper;

network_agent::network_agent() {}

void network_agent::set_config_dir(std::string&& dir) {
  log::trace_fmt("set_config_dir: {}", dir);
}

void network_agent::set_on_printer_connected_fn(BBL::OnPrinterConnectedFn&& fn) {
  this->on_printer_connected_fn_ = std::move(fn);
}

void network_agent::set_on_server_connected_fn(BBL::OnServerConnectedFn&& fn) {
  this->on_server_connected_fn_ = std::move(fn);
}

void network_agent::set_on_local_connected_fn(BBL::OnLocalConnectedFn&& fn) {
  this->on_local_connected_fn_ = std::move(fn);
}

void network_agent::set_on_local_message_fn(BBL::OnMessageFn&& fn) {
  this->on_local_message_fn_ = std::move(fn);
}

std::string network_agent::get_user_selected_machine() {
  return "";
}

void network_agent::set_user_selected_machine(std::string dev_id) {
  (void)dev_id;
}

void network_agent::start() {
  auto scope = BBZL_TRACE_SCOPE();
}

void network_client::on_connect(int reason_code) {
  auto scope = BBZL_TRACE_SCOPE(reason_code);

  this->report_sub_mid = this->mosqpp->subscribe("device/" + this->dev_id + "/report", 1);

  // TODO(?): return_code = 5: "login information expired"
  this->agent->on_server_connected_fn_(0, reason_code);

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
  mosquitto_message const* msg
) {
  auto msg_str = std::string((char*)msg->payload, msg->payloadlen);
  log::debug_fmt("({}) received message ({}): {}", this->dev_id, msg->topic, msg_str);
  this->agent->on_local_message_fn_(this->dev_id, msg_str);
}

void network_client::on_suback(mqtt::message_id mid) {
  log::debug_fmt("received suback for mid {}", mid);

  if (mid == this->report_sub_mid) {
    this->report_sub_mid = 0;
    log::debug_fmt(
      "({}) subscribed to topic: device/{}/request",
      this->dev_id,
      this->dev_id
    );
    this->agent->on_local_connected_fn_(0, this->dev_id, "ssl:");
    return;
  }

  log::error_fmt(
    "({}) received unexpected suback for mid {}",
    this->dev_id,
    mid
  );
}

std::optional<network_client*> network_agent::get_conn(std::string const& dev_id) {
  auto pos = this->connections_.find(dev_id);
  if (pos != this->connections_.end()) {
    return pos->second;
  }

  return std::nullopt;
}

network_client* network_agent::get_or_create_conn(std::string const& dev_id) {
  auto pos = this->connections_.find(dev_id);
  if (pos != this->connections_.end()) {
    return pos->second;
  }

  auto conn = new network_client{
    .agent = this,
    .dev_id = dev_id,
    .mosqpp = new mqtt::mosquittopp(),
  };

  conn->mosqpp->set_on_connect_cb(
    wrap_try([conn](mqtt::mosquittopp&, int reason_code) {
      conn->on_connect(reason_code);
    })
  );
  conn->mosqpp->set_on_disconnect_cb(
    wrap_try([conn](mqtt::mosquittopp&, int reason_code) {
      conn->on_disconnect(reason_code);
    })
  );
  conn->mosqpp->set_on_suback_cb(
    wrap_try([conn](
      mqtt::mosquittopp&,
      mqtt::message_id mid,
      const std::span<const int>& qos_arr
    ) {
      (void)qos_arr;
      conn->on_suback(mid);
    })
  );
  conn->mosqpp->set_on_message_cb(
    wrap_try([conn](
      mqtt::mosquittopp&,
      mosquitto_message const* msg
    ) {
      conn->on_message(msg);
    })
  );

  auto const& [_, emplaced] = this->connections_.emplace(dev_id, conn);
  assert(emplaced);
  return conn;
}

void network_agent::connect(
  std::string const& dev_id,
  std::string const& dev_ip,
  std::string const& username,
  std::string const& password,
  bool use_ssl
) {
  (void)use_ssl; // TODO?
  auto scope = BBZL_TRACE_SCOPE(dev_id, dev_ip, username, "*PASSWORD*"sv);
  auto conn = this->get_or_create_conn(dev_id);
  if (conn->mosqpp->connected()) {
    log::debug_fmt(
      "({}) connect called while already connected; requesting disconnect",
      dev_id
    );
    conn->mosqpp->disconnect();
    while (conn->mosqpp->active()) {
      log::debug_fmt(
        "({}) waiting for mosquitto thread to terminate",
        dev_id
      );
      std::this_thread::sleep_for(200ms);
    }
  }
  conn->mosqpp->connect(dev_ip, 8883, username, password, 60);
}

void network_agent::send_message(
  std::string const& dev_id,
  std::string const& json_str,
  int qos
) {
  auto scope = BBZL_TRACE_SCOPE(dev_id, json_str, qos);
  auto conn = this->get_conn(dev_id);
  if (!conn.has_value()) {
    throw std::runtime_error(std::format(
      "network client doesn't exist: {}",
      dev_id
    ));
  }

  (*conn)->mosqpp->publish("device/" + dev_id + "/request", json_str, qos);
}

}

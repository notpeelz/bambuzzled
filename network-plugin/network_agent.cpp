#include <cassert>
#include <format>
#include <span>
#include <chrono>
#include <thread>
#include <utility>
#include <string>
#include <string_view>
#include <optional>
#include <exception>
#include <stdexcept>
#include <bambuzzled/log.hpp>
#include <bambuzzled/try_wrapper.hpp>
#include <bambuzzled/networking/api/bambu_networking.hpp>
#include "mosquittopp.hpp"
#include "network_agent.hpp"

namespace {

using namespace bbzl::try_wrapper;

}

namespace bbzl {

using namespace std::chrono_literals;
using namespace std::string_view_literals;

network_agent::network_agent() {}

void network_agent::set_config_dir(std::string&& dir) noexcept {
  bbzl::log::trace_fmt("set_config_dir: {}", dir);
}

void network_agent::set_on_printer_connected_fn(BBL::OnPrinterConnectedFn&& fn) noexcept {
  this->on_printer_connected_fn_ = fn == nullptr
    ? no_op
    : std::move(fn);
}

void network_agent::set_on_server_connected_fn(BBL::OnServerConnectedFn&& fn) noexcept {
  this->on_server_connected_fn_ = fn == nullptr
    ? no_op
    : std::move(fn);
}

void network_agent::set_on_local_connected_fn(BBL::OnLocalConnectedFn&& fn) noexcept {
  this->on_local_connected_fn_ = fn == nullptr
    ? no_op
    : std::move(fn);
}

void network_agent::set_on_local_message_fn(BBL::OnMessageFn&& fn) noexcept {
  this->on_local_message_fn_ = fn == nullptr
    ? no_op
    : std::move(fn);
}

void network_agent::set_queue_on_main_fn(BBL::QueueOnMainFn&& fn) noexcept {
  this->queue_on_main_fn_ = fn == nullptr
    ? no_op
    : std::move(fn);
}

std::string network_agent::get_user_selected_machine() noexcept {
  return "";
}

void network_agent::set_user_selected_machine(std::string dev_id) noexcept {
  (void)dev_id;
}

void network_agent::start() {
  auto scope = BBZL_TRACE_SCOPE();
}

void network_client::on_connect(int reason_code) {
  auto scope = BBZL_TRACE_SCOPE(reason_code);

  if (reason_code != 0) {
    bbzl::log::error_fmt(
      "failed connecting to MQTT broker: {}",
      bbzl::mqtt::mosquitto_connack_string(reason_code)
    );
  } else {
    bbzl::log::info_fmt(
      "({}) connected from MQTT broker",
      this->dev_id
    );
  }

  // TODO: urlencode dev_id?
  this->report_sub_mid = this->mosqpp->subscribe("device/" + this->dev_id + "/report", 1);

  // TODO(?): return_code = 5: "login information expired"
  try {
    this->agent->on_server_connected_fn_(0, reason_code);
  } catch (...) {}
}

void network_client::on_disconnect(int reason_code) {
  bbzl::log::info_fmt(
    "({}) disconnected from MQTT broker",
    this->dev_id
  );
  if (reason_code != 0) {
    bbzl::log::error_fmt(
      "unexpected disconnect: {}",
      bbzl::mqtt::mosquitto_strerror(reason_code)
    );
  }
}

void network_client::on_message(mqtt::message const* msg) {
  auto msg_str = std::string((char*)msg->payload, msg->payloadlen);
  bbzl::log::debug_fmt(
    "({}) received message ({}): {}",
    this->dev_id,
    msg->topic,
    msg_str
  );
  try {
    this->agent->on_local_message_fn_(this->dev_id, msg_str);
  } catch (...) {}
}

void network_client::on_suback(mqtt::message_id mid) {
  bbzl::log::debug_fmt("received suback for mid {}", mid);

  if (mid == this->report_sub_mid) {
    this->report_sub_mid = 0;
    bbzl::log::debug_fmt(
      "({}) subscribed to topic: device/{}/request",
      this->dev_id,
      this->dev_id
    );
    try {
      this->agent->on_local_connected_fn_(0, this->dev_id, "ssl:");
    } catch (...) {}
    return;
  }

  bbzl::log::error_fmt(
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
    .mosqpp = new bbzl::mqtt::mosquittopp(),
  };

  conn->mosqpp->set_on_connect_cb(
    wrap_try([conn](bbzl::mqtt::mosquittopp&, int reason_code) {
      conn->on_connect(reason_code);
    })
  );
  conn->mosqpp->set_on_disconnect_cb(
    wrap_try([conn](bbzl::mqtt::mosquittopp&, int reason_code) {
      conn->on_disconnect(reason_code);
    })
  );
  conn->mosqpp->set_on_suback_cb(
    wrap_try([conn](
      bbzl::mqtt::mosquittopp&,
      bbzl::mqtt::message_id mid,
      const std::span<const int>& qos_arr
    ) {
      (void)qos_arr;
      conn->on_suback(mid);
    })
  );
  conn->mosqpp->set_on_message_cb(
    wrap_try([conn](
      bbzl::mqtt::mosquittopp&,
      bbzl::mqtt::message const* msg
    ) {
      conn->on_message(msg);
    })
  );

  auto const& [_, emplaced] = this->connections_.emplace(dev_id, conn);
  assert(emplaced);
  return conn;
}

void network_agent::disconnect() {
  for (auto& [_, conn] : this->connections_) {
    (void)this->disconnect(conn->dev_id);
  }
}

bool network_agent::disconnect(std::string const& dev_id) {
  auto maybe_conn = this->get_conn(dev_id);
  if (!maybe_conn.has_value()) {
    throw network_client_not_found(dev_id);
  }
  auto conn = *maybe_conn;

  if (!conn->mosqpp->connected()) {
    return true;
  }

  try {
    conn->mosqpp->disconnect();
    while (conn->mosqpp->active()) {
      bbzl::log::debug_fmt(
        "({}) waiting for mosquitto thread to terminate",
        dev_id
      );
      std::this_thread::sleep_for(200ms);
    }
  } catch (std::exception& ex) {
    bbzl::log::error_fmt("mosquitto disconnect failed: {}", ex.what());
    return false;
  }

  return true;
}

bool network_agent::connect(
  std::string const& dev_id,
  std::string const& dev_ip,
  std::string const& username,
  std::string const& password,
  bool use_ssl
) {
  auto scope = BBZL_TRACE_SCOPE(dev_id, dev_ip, username, "*PASSWORD*"sv);

  if (!use_ssl) {
    bbzl::log::warn(
      "attempted to connect without SSL (not implemented);"
      " connecting with SSL anyway"
    );
  }

  auto conn = this->get_or_create_conn(dev_id);
  try {
    if (conn->mosqpp->connected()) {
      log::debug_fmt(
        "({}) connect called while already connected; requesting disconnect",
        dev_id
      );
      (void)this->disconnect(dev_id);
    }

    conn->mosqpp->connect(dev_ip, 8883, username, password, 60);
  } catch (std::exception& ex) {
    bbzl::log::error_fmt("mosquitto connect failed: {}", ex.what());
    try {
      this->on_local_connected_fn_(1, dev_id, "-1");
    } catch (...) {}
    return false;
  }

  return true;
}

bool network_agent::send_message(
  std::string const& dev_id,
  std::string const& json_str,
  int qos
) {
  auto scope = BBZL_TRACE_SCOPE(dev_id, json_str, qos);
  auto maybe_conn = this->get_conn(dev_id);
  if (!maybe_conn.has_value()) {
    throw network_client_not_found(dev_id);
  }
  auto conn = *maybe_conn;

  try {
    // TODO: urlencode dev_id?
    (void)conn->mosqpp->publish("device/" + dev_id + "/request", json_str, qos);
  } catch (std::exception& ex) {
    bbzl::log::error_fmt("mosquitto publish failed: {}", ex.what());
    return false;
  }

  return true;
}

}

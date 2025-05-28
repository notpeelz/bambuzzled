#include <mutex>
#include <format>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <cassert>
#include <optional>
#include <stdexcept>
#include <exception>
#include <shared_mutex>
#include <bambuzzled/log.hpp>
#include <bambuzzled/curl.hpp>
#include <bambuzzled/try_wrapper.hpp>
#include <bambuzzled/optional_ref.hpp>
#include <bambuzzled/networking/api/bambu_networking.hpp>
#include "mosquittopp.hpp"
#include "network_agent.hpp"

namespace bbzl {

using namespace bbzl::utils::try_wrapper;

network_agent::network_agent() {}

void network_agent::set_config_dir(std::string&& dir) noexcept {
  bbzl::log::trace_fmt("set_config_dir: {}", dir);
}

void network_agent::set_on_printer_connected_fn(
  BBL::OnPrinterConnectedFn&& fn
) noexcept {
  auto cb_lock = std::unique_lock(this->cb_mutex_);
  this->on_printer_connected_fn_ = fn == nullptr
    ? [](...) {}
    : std::move(fn);
}

void network_agent::set_on_local_connected_fn(
  BBL::OnLocalConnectedFn&& fn
) noexcept {
  auto cb_lock = std::unique_lock(this->cb_mutex_);
  this->on_local_connected_fn_ = fn == nullptr
    ? [](...) {}
    : std::move(fn);
}

void network_agent::set_on_local_message_fn(
  BBL::OnMessageFn&& fn
) noexcept {
  auto cb_lock = std::unique_lock(this->cb_mutex_);
  this->on_local_message_fn_ = fn == nullptr
    ? [](...) {}
    : std::move(fn);
}

void network_agent::set_queue_on_main_fn(
  BBL::QueueOnMainFn&& fn
) noexcept {
  auto cb_lock = std::unique_lock(this->cb_mutex_);
  this->queue_on_main_fn_ = fn == nullptr
    ? [](...) {}
    : std::move(fn);
}

std::string network_agent::get_user_selected_machine() noexcept {
  return "";
}

void network_agent::set_user_selected_machine(
  std::string dev_id
) noexcept {
  (void)dev_id;
}

void network_client::on_connect(int reason_code) {
  auto client_lock = std::unique_lock(this->mutex_);

  if (reason_code != 0) {
    bbzl::log::error_fmt(
      "failed connecting to printer: {}",
      bbzl::mqtt::mosquitto_connack_string(reason_code)
    );
  } else {
    bbzl::log::info_fmt(
      "({}) connected to printer",
      this->id_
    );
  }

  auto topic = std::format(
    "device/{}/report",
    bbzl::utils::curl::urlencode(this->id_)
  );
  this->report_sub_mid_ = this->mosqpp_.subscribe(topic, 1);
}

void network_client::on_disconnect(int reason_code) {
  bbzl::log::info_fmt(
    "({}) disconnected from printer",
    this->id_
  );
  if (reason_code != 0) {
    bbzl::log::error_fmt(
      "unexpected disconnect: {}",
      bbzl::mqtt::mosquitto_strerror(reason_code)
    );
  }
}

void network_client::on_message(mqtt::message const* msg) {
  BBL::OnMessageFn on_local_message;
  {
    auto cb_lock = std::shared_lock(this->agent_->cb_mutex_);
    on_local_message = this->agent_->on_local_message_fn_;
  }

  auto msg_str = std::string((char*)msg->payload, msg->payloadlen);
  bbzl::log::debug_fmt(
    "({}) received message ({}): {}",
    this->id_,
    msg->topic,
    msg_str
  );
  try {
    on_local_message(this->id_, msg_str);
  } catch (std::exception& ex) {
    bbzl::log::error_fmt(
      "caught exception from on_local_message callback: {}",
      ex.what()
    );
  } catch (...) {
    bbzl::log::error_fmt(
      "caught exception from on_local_message callback"
    );
  }
}

void network_client::on_suback(mqtt::message_id mid) {
  auto client_lock = std::shared_lock(this->mutex_);

  BBL::OnLocalConnectedFn on_local_connected;
  {
    auto cb_lock = std::shared_lock(this->agent_->cb_mutex_);
    on_local_connected = this->agent_->on_local_connected_fn_;
  }

  bbzl::log::debug_fmt("received suback for mid {}", mid);

  if (mid == this->report_sub_mid_) {
    this->report_sub_mid_ = 0;
    bbzl::log::debug_fmt(
      "({}) subscribed to topic: device/{}/request",
      this->id_,
      this->id_
    );
    try {
      on_local_connected(0, this->id_, "ssl:");
    } catch (std::exception& ex) {
      bbzl::log::error_fmt(
        "caught exception from on_local_connected callback: {}",
        ex.what()
      );
    } catch (...) {
      bbzl::log::error_fmt(
        "caught exception from on_local_connected callback"
      );
    }
    return;
  }

  bbzl::log::error_fmt(
    "({}) received unexpected suback for mid {}",
    this->id_,
    mid
  );
}

std::optional<std::shared_ptr<network_client>>
network_agent::get_conn(std::string const& dev_id) {
  auto pos = this->connections_.find(dev_id);
  if (pos != this->connections_.end()) {
    return pos->second;
  }

  return std::nullopt;
}

// HACK: workaround for make_shared being unable to
// access the private ctor
class network_client_private
  : public network_client
{
public:
  template<typename... Args>
  network_client_private(Args&&... args)
    : network_client(std::forward<Args>(args)...) {}
};

void network_agent::create_conn(std::string const& dev_id) {
  auto pos = this->connections_.find(dev_id);
  if (pos != this->connections_.end()) {
    throw invalid_network_client("client id already exists");
  }

  auto client_dummy =
    std::make_shared<network_client_private>(this, dev_id);
  auto client =
    static_cast<std::shared_ptr<network_client>>(client_dummy);
  auto const& [_, emplaced] =
    this->connections_.emplace(dev_id, client);
  assert(emplaced);
}

void network_agent::disconnect() {
  auto conn_lock = std::unique_lock(this->connections_mutex_);
  std::vector<std::string const*> failed_ids;

  for (auto& [_, client] : this->connections_) {
    auto client_lock = std::unique_lock(client->mutex_);
    if (!client->mosqpp_.connected()) {
      continue;
    }

    try {
      client->mosqpp_.disconnect();
    } catch (std::exception& ex) {
      bbzl::log::error_fmt(
        "failed to disconnect client {}: {}",
        client->id_,
        ex.what()
      );
      failed_ids.push_back(&client->id_);
    }
  }

  if (failed_ids.empty()) {
    return;
  }

  std::ostringstream oss;
  oss << "failed to disconnect client(s): ";
  for (auto it = failed_ids.begin(); it != failed_ids.end(); ++it) {
    if (it != failed_ids.begin()) {
      oss << ", ";
    }
    oss << **it;
  }

  throw std::runtime_error(oss.str());
}

bool network_agent::disconnect(std::string const& dev_id) {
  std::optional<std::shared_ptr<network_client>> maybe_client;
  {
    auto conn_lock = std::shared_lock(this->connections_mutex_);
    maybe_client = this->get_conn(dev_id);
  }

  if (!maybe_client.has_value()) {
    throw invalid_network_client(dev_id);
  }
  auto& client = *maybe_client;

  auto client_lock = std::unique_lock(client->mutex_);
  if (!client->mosqpp_.connected()) {
    return true;
  }

  client->mosqpp_.disconnect();

  return true;
}

void network_agent::connect(
  std::string const& dev_id,
  std::string const& dev_ip,
  std::string const& username,
  std::string const& password,
  bool use_ssl
) {
  if (!use_ssl) {
    bbzl::log::warn(
      "attempted to connect without SSL (not implemented);"
      " connecting with SSL anyway"
    );
  }

  std::optional<std::shared_ptr<network_client>> maybe_client;
  {
    auto conn_lock = std::unique_lock(this->connections_mutex_);
    maybe_client = this->get_conn(dev_id);
    if (!maybe_client.has_value()) {
      this->create_conn(dev_id);
    }
    maybe_client = this->get_conn(dev_id);
    assert(maybe_client.has_value());
  }

  auto& client = *maybe_client;
  auto client_lock = std::unique_lock(client->mutex_);

  try {
    if (client->mosqpp_.connected()) {
      log::debug_fmt(
        "({}) connect called while already connected;"
        " requesting disconnect",
        dev_id
      );
      (void)client->mosqpp_.disconnect();
    }

    client->mosqpp_.connect(dev_ip, 8883, username, password, 60);
  } catch (std::exception& ex) {
    bbzl::log::error_fmt("mosquitto connect failed: {}", ex.what());

    BBL::OnLocalConnectedFn on_local_connected;
    {
      auto cb_lock = std::unique_lock(this->cb_mutex_);
      on_local_connected = this->on_local_connected_fn_;
    }

    try {
      on_local_connected(1, dev_id, "-1");
    } catch (std::exception& ex) {
      bbzl::log::error_fmt(
        "caught exception from on_local_connected callback: {}",
        ex.what()
      );
    } catch (...) {
      bbzl::log::error_fmt(
        "caught exception from on_local_connected callback"
      );
    }
  }
}

void network_agent::send_message(
  std::string const& dev_id,
  std::string const& msg,
  int qos
) {
  auto conn_lock = std::shared_lock(this->connections_mutex_);
  auto maybe_client = this->get_conn(dev_id);
  if (!maybe_client.has_value()) {
    throw invalid_network_client(dev_id);
  }
  auto& client = *maybe_client;

  // NOTE: we check if we're connected before locking, otherwise we might
  // deadlock if another thread is blocking on mosquitto_connect.
  if (!client->mosqpp_.connected()) {
    throw invalid_network_client(dev_id);
  }

  auto client_lock = std::unique_lock(client->mutex_);
  auto topic = std::format(
    "device/{}/request",
    bbzl::utils::curl::urlencode(dev_id)
  );
  bbzl::log::debug_fmt(
    "({}) sending message ({}): {}",
    dev_id,
    topic,
    msg
  );
  (void)client->mosqpp_.publish(topic, msg, qos);
}

}

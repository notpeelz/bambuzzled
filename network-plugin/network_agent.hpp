#pragma once

#include <map>
#include <mutex>
#include <string>
#include <optional>
#include <stdexcept>
#include <bambuzzled/networking/api/bambu_networking.hpp>
#include "mosquittopp.hpp"

namespace bbzl {

class network_agent;

class network_client {
  friend network_agent;

private:
  void on_connect(int reason_code);
  void on_disconnect(int reason_code);
  void on_message(bbzl::mqtt::message const* msg);
  void on_suback(bbzl::mqtt::message_id mid);

public:
  network_agent* agent;
  std::string dev_id;
  bbzl::mqtt::message_id report_sub_mid;
  bbzl::mqtt::mosquittopp* mosqpp;
  ~network_client() {
    if (this->mosqpp != nullptr) {
      delete this->mosqpp;
    }
  }
};

class network_client_not_found
  : public std::runtime_error
{
public:
  network_client_not_found(std::string const& id)
    : std::runtime_error("network client doesn't exist: " + id) {}
};

constexpr static auto no_op = [](...) {};

class network_agent {
  friend network_client;
private:
  std::string config_dir_;
  std::recursive_mutex mosq_mutex_;
  std::recursive_mutex cb_mutex_;
  std::map<std::string, network_client*> connections_;
  BBL::OnPrinterConnectedFn on_printer_connected_fn_ = no_op;
  BBL::OnLocalConnectedFn on_local_connected_fn_ = no_op;
  BBL::OnMessageFn on_local_message_fn_ = no_op;
  BBL::QueueOnMainFn queue_on_main_fn_ = no_op;
  network_client* get_or_create_conn(std::string const& dev_id);
  std::optional<network_client*> get_conn(std::string const& dev_id);

public:
  network_agent();
  void set_config_dir(std::string&& dir) noexcept;
  void set_on_printer_connected_fn(BBL::OnPrinterConnectedFn&& fn) noexcept;
  void set_on_local_connected_fn(BBL::OnLocalConnectedFn&& fn) noexcept;
  void set_on_local_message_fn(BBL::OnMessageFn&& fn) noexcept;
  void set_queue_on_main_fn(BBL::QueueOnMainFn&& fn) noexcept;
  [[nodiscard]]
  std::string get_user_selected_machine() noexcept;
  void set_user_selected_machine(std::string dev_id) noexcept;
  void disconnect();
  [[nodiscard]]
  bool disconnect(std::string const& dev_id);
  [[nodiscard]]
  bool connect(
    std::string const& dev_id,
    std::string const& dev_ip,
    std::string const& username,
    std::string const& password,
    bool use_ssl
  );
  [[nodiscard]]
  bool send_message(
    std::string const& dev_id,
    std::string const& msg,
    int qos
  );
};

}

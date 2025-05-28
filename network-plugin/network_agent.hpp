#pragma once

#include <map>
#include <span>
#include <string>
#include <format>
#include <memory>
#include <utility>
#include <optional>
#include <stdexcept>
#include <shared_mutex>
#include <bambuzzled/try_wrapper.hpp>
#include <bambuzzled/networking/api/bambu_networking.hpp>
#include "mosquittopp.hpp"

namespace bbzl {

class network_agent;

class network_client {
  friend network_agent;

private:
  std::shared_mutex mutex_;
  network_agent* agent_ = nullptr;
  std::string id_;
  bbzl::mqtt::message_id report_sub_mid_;
  bbzl::mqtt::mosquittopp mosqpp_;
  bool connect_done_ = false;

  void on_connect(int reason_code);
  void on_disconnect(int reason_code);
  void on_message(bbzl::mqtt::message const* msg);
  void on_suback(bbzl::mqtt::message_id mid);

protected:
  network_client(network_agent* agent, std::string id)
    : agent_(agent),
      id_(std::move(id))
  {
    using namespace bbzl::utils::try_wrapper;

    this->mosqpp_.set_on_connect_cb(
      wrap_try([this](bbzl::mqtt::mosquittopp&, int reason_code) {
        this->on_connect(reason_code);
      })
    );

    this->mosqpp_.set_on_disconnect_cb(
      wrap_try([this](bbzl::mqtt::mosquittopp&, int reason_code) {
        this->on_disconnect(reason_code);
      })
    );

    this->mosqpp_.set_on_suback_cb(
      wrap_try([this](
        bbzl::mqtt::mosquittopp&,
        bbzl::mqtt::message_id mid,
        const std::span<const int>& qos_arr
      ) {
        (void)qos_arr;
        this->on_suback(mid);
      })
    );

    this->mosqpp_.set_on_message_cb(
      wrap_try([this](
        bbzl::mqtt::mosquittopp&,
        bbzl::mqtt::message const* msg
      ) {
        this->on_message(msg);
      })
    );
  }
};

class invalid_network_client
  : public std::runtime_error
{
public:
  invalid_network_client(std::string const& id)
    : std::runtime_error(std::format(
      "network client doesn't exist or isn't connected: {}",
      id
    )) {}
};

class network_agent {
  friend network_client;
private:
  std::string config_dir_;
  std::shared_mutex connections_mutex_;
  std::shared_mutex cb_mutex_;
  std::map<std::string, std::shared_ptr<network_client>> connections_;
  BBL::OnPrinterConnectedFn on_printer_connected_fn_ = [](...) {};
  BBL::OnLocalConnectedFn on_local_connected_fn_ = [](...) {};
  BBL::OnMessageFn on_local_message_fn_ = [](...) {};
  BBL::QueueOnMainFn queue_on_main_fn_ = [](...) {};
  void create_conn(std::string const& dev_id);
  std::optional<std::shared_ptr<network_client>> get_conn(
    std::string const& dev_id
  );

public:
  network_agent();
  void set_config_dir(std::string&& dir) noexcept;
  void set_on_printer_connected_fn(
    BBL::OnPrinterConnectedFn&& fn
  ) noexcept;
  void set_on_local_connected_fn(
    BBL::OnLocalConnectedFn&& fn
  ) noexcept;
  void set_on_local_message_fn(BBL::OnMessageFn&& fn) noexcept;
  void set_queue_on_main_fn(BBL::QueueOnMainFn&& fn) noexcept;
  [[nodiscard]]
  std::string get_user_selected_machine() noexcept;
  void set_user_selected_machine(std::string dev_id) noexcept;
  void disconnect();
  bool disconnect(std::string const& dev_id);
  void connect(
    std::string const& dev_id,
    std::string const& dev_ip,
    std::string const& username,
    std::string const& password,
    bool use_ssl
  );
  void send_message(
    std::string const& dev_id,
    std::string const& msg,
    int qos
  );
};

}

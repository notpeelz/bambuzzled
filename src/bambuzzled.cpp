#include <type_traits>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>

#include <nlohmann/json.hpp>
#include <bambuzzled/plugin_exports.hpp>

#include "log.hpp"
#include "network_agent.hpp"

using network_agent = ::bbzl::network_agent;

namespace {

namespace log = ::bbzl::log;

template<typename F>
static void try_(F fn) requires std::invocable<F> {
  try {
    fn();
  } catch (std::exception& ex) {
    log::error("{}", ex.what());
  }
}

template<typename F>
static auto try_or_default(
  F fn
) -> typename std::invoke_result_t<F> requires std::invocable<F> {
  try {
    return fn();
  } catch (std::exception& ex) {
    log::error("{}", ex.what());
    return typename std::invoke_result_t<F>{};
  }
}

template<typename F>
static auto try_or(
  typename std::invoke_result_t<F> fallback,
  F fn
) -> typename std::invoke_result_t<F> requires std::invocable<F> {
  try {
    return fn();
  } catch (std::exception& ex) {
    log::error("{}", ex.what());
    return fallback;
  }
}

}

bool bambu_network_check_debug_consistent(bool is_debug) {
  (void)is_debug;
  return true;
}

std::string bambu_network_get_version(void) {
  return BAMBU_NETWORK_AGENT_VERSION;
}

void* bambu_network_create_agent(std::string log_dir) {
  (void)log_dir;
  return try_or(nullptr, []() {
    return new network_agent();
  });
}

int bambu_network_destroy_agent(void* agentp) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  try_([&]() {
    delete agent;
  });
  return 0;
}

int bambu_network_start(void* agentp) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&agent]() {
    return agent->start();
  });
}

bool bambu_network_start_discovery(void* agentp, bool start, bool sending) {
  (void)agentp;
  (void)start;
  (void)sending;
  return false;
}

std::string bambu_network_get_user_selected_machine(void* agentp) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or_default([&]() {
    return agent->get_user_selected_machine();
  });
}

int bambu_network_set_user_selected_machine(void* agentp, std::string dev_id) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_user_selected_machine(dev_id);
    return 0;
  });
}

int bambu_network_connect_printer(
  void* agentp,
  std::string dev_id,
  std::string dev_ip,
  std::string username,
  std::string password,
  bool use_ssl
) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    return agent->connect_printer(dev_id, dev_ip, username, password, use_ssl);
  });
}

int bambu_network_send_message_to_printer(void* agentp, std::string dev_id, std::string json_str, int qos) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    return agent->send_message_to_printer(dev_id, json_str, qos);
  });
}

int bambu_network_disconnect_printer(void* agentp) {
  (void)agentp;
  return 0;
}

int bambu_network_set_config_dir(void* agentp, std::string config_dir) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_config_dir(std::move(config_dir));
    return 0;
  });
}

int bambu_network_set_on_printer_connected_fn(void* agentp, OnPrinterConnectedFn fn) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_on_printer_connected_fn(std::move(fn));
    return 0;
  });
}

int bambu_network_set_on_server_connected_fn(void* agentp, OnServerConnectedFn fn) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_on_server_connected_fn(std::move(fn));
    return 0;
  });
}

int bambu_network_set_on_local_connect_fn(void* agentp, OnLocalConnectedFn fn) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_on_local_connected_fn(std::move(fn));
    return 0;
  });
}

int bambu_network_set_on_local_message_fn(void* agentp, OnMessageFn fn) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_on_local_message_fn(std::move(fn));
    return 0;
  });
}

std::string bambu_network_build_logout_cmd(void* agentp) {
  (void)agentp;
  return try_or_default([]() {
    auto json = R"({
      "command": "studio_useroffline",
      "sequence_id": "10001"
    })"_json;
    return json.dump();
  });
}

int bambu_network_start_print(
  void* agentp,
  PrintParams params,
  OnUpdateStatusFn update_fn,
  WasCancelledFn cancel_fn,
  OnWaitFn wait_fn
) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  (void)wait_fn;
  return 0;
}

int bambu_network_start_local_print_with_record(
  void* agentp,
  PrintParams params,
  OnUpdateStatusFn update_fn,
  WasCancelledFn cancel_fn,
  OnWaitFn wait_fn
) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  (void)wait_fn;
  return 0;
}

int bambu_network_start_send_gcode_to_sdcard(
  void* agentp,
  PrintParams params,
  OnUpdateStatusFn update_fn,
  WasCancelledFn cancel_fn,
  OnWaitFn wait_fn
) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  (void)wait_fn;
  return 0;
}

int bambu_network_start_local_print(
  void* agentp,
  PrintParams params,
  OnUpdateStatusFn update_fn,
  WasCancelledFn cancel_fn
) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  return 0;
}

int bambu_network_start_sdcard_print(
  void* agentp,
  PrintParams params,
  OnUpdateStatusFn update_fn,
  WasCancelledFn cancel_fn
) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  return 0;
}

#include <type_traits>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>

#include <nlohmann/json.hpp>

#include "network_agent.hpp"
#include "bambuzzled.hpp"

template<typename F>
static void try_(F fn) requires std::invocable<F> {
  try {
    fn();
  } catch (std::exception& ex) {
    std::cerr << "ERROR: " << ex.what() << std::endl;
  }
}

template<typename F>
static auto try_or_default(
  F fn
) -> typename std::invoke_result_t<F> requires std::invocable<F> {
  try {
    return fn();
  } catch (std::exception& ex) {
    std::cerr << "ERROR: " << ex.what() << std::endl;
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
    std::cerr << "ERROR: " << ex.what() << std::endl;
    return fallback;
  }
}

typedef bool (*func_check_debug_consistent)(bool is_debug);
bool bambu_network_check_debug_consistent(bool is_debug) {
  (void)is_debug;
  return true;
}

typedef std::string (*func_get_version)(void);
std::string bambu_network_get_version(void) {
  return BAMBU_NETWORK_AGENT_VERSION;
}

typedef void* (*func_create_agent)(std::string log_dir);
void* bambu_network_create_agent(std::string&& log_dir) {
  (void)log_dir;
  return try_or(nullptr, []() {
    return new network_agent();
  });
}

typedef int (*func_destroy_agent)(void* agentp);
int bambu_network_destroy_agent(void* agentp) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  try_([&]() {
    delete agent;
  });
  return 0;
}

typedef int (*func_start)(void* agentp);
int bambu_network_start(void* agentp) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&agent]() {
    return agent->start();
  });
}

typedef bool (*func_start_discovery)(void* agentp, bool start, bool sending);
bool bambu_network_start_discovery(void* agentp, bool start, bool sending) {
  (void)agentp;
  (void)start;
  (void)sending;
  std::cerr << "start_discovery" << std::endl;
  return 1;
}

typedef std::string (*func_get_user_selected_machine)(void* agentp);
std::string bambu_network_get_user_selected_machine(void* agentp) {
  (void)agentp;
  // TODO
  std::cerr << "get_user_selected_machine" << std::endl;
  return "";
}

typedef int (*func_set_user_selected_machine)(void* agentp, std::string dev_id);
int bambu_network_set_user_selected_machine(void* agentp, std::string&& dev_id) {
  (void)agentp;
  (void)dev_id;
  // TODO
  std::cerr << "set_user_selected_machine: " << dev_id << std::endl;
  return 0;
}

typedef int (*func_connect_printer)(
  void* agentp,
  std::string dev_id,
  std::string dev_ip,
  std::string username,
  std::string password,
  bool use_ssl
);
int bambu_network_connect_printer(
  void* agentp,
  std::string&& dev_id,
  std::string&& dev_ip,
  std::string&& username,
  std::string&& password,
  bool use_ssl
) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    return agent->connect_printer(dev_id, dev_ip, username, password, use_ssl);
  });
}

typedef int (*func_send_message_to_printer)(void* agentp, std::string dev_id, std::string json_str, int qos);
int bambu_network_send_message_to_printer(void* agentp, std::string&& dev_id, std::string&& json_str, int qos) {
  (void)agentp;
  (void)dev_id;
  (void)json_str;
  (void)qos;
  // TODO
  std::cerr << "send_message_to_printer(" << dev_id << "): " << json_str << std::endl;

  return 0;
}

typedef int (*func_disconnect_printer)(void* agentp);
int bambu_network_disconnect_printer(void* agentp) {
  (void)agentp;
  return 0;
}

typedef int (*func_set_config_dir)(void* agentp, std::string config_dir);
int bambu_network_set_config_dir(void* agentp, std::string&& config_dir) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_config_dir(std::move(config_dir));
    return 0;
  });
}

typedef int (*func_set_on_printer_connected_fn)(void* agentp, OnPrinterConnectedFn fn);
int bambu_network_set_on_printer_connected_fn(void* agentp, OnPrinterConnectedFn&& fn) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_on_printer_connected_fn(std::move(fn));
    return 0;
  });
}

typedef int (*func_set_on_server_connected_fn)(void* agentp, OnServerConnectedFn fn);
int bambu_network_set_on_server_connected_fn(void* agentp, OnServerConnectedFn&& fn) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_on_server_connected_fn(std::move(fn));
    return 0;
  });
}

typedef int (*func_set_on_local_connect_fn)(void* agentp, OnLocalConnectedFn fn);
int bambu_network_set_on_local_connect_fn(void* agentp, OnLocalConnectedFn&& fn) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_on_local_connected_fn(std::move(fn));
    return 0;
  });
}

typedef int (*func_set_on_local_message_fn)(void* agentp, OnMessageFn fn);
int bambu_network_set_on_local_message_fn(void* agentp, OnMessageFn&& fn) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_on_local_message_fn(std::move(fn));
    return 0;
  });
}

typedef std::string (*func_build_logout_cmd)(void* agentp);
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

#include <type_traits>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>

#include <nlohmann/json.hpp>

#include "network_agent.hpp"
#include "bambuzzled.hpp"

template<typename T, typename F>
static T try_or_default(F fn) requires std::invocable<F> {
  try {
    return fn();
  } catch (std::exception& ex) {
    std::cerr << "ERROR: " << ex.what() << std::endl;
    return T();
  }
}

template<typename T, typename F>
static T try_or(T fallback, F fn) requires std::invocable<F> {
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
  return new network_agent();
}

typedef int (*func_destroy_agent)(void* agentp);
int bambu_network_destroy_agent(void* agentp) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  delete agent;
  return 0;
}

typedef int (*func_set_config_dir)(void* agentp, std::string config_dir);
int bambu_network_set_config_dir(void* agentp, std::string&& config_dir) {
  auto agent = reinterpret_cast<network_agent*>(agentp);
  return try_or(1, [&agent, &config_dir]() {
    agent->set_config_dir(config_dir);
    return 0;
  });
}

typedef std::string (*func_build_logout_cmd)(void* agentp);
std::string bambu_network_build_logout_cmd(void* agentp) {
  (void)agentp;
  return try_or_default<std::string>([]() {
    auto json = R"({
      "command": "studio_useroffline",
      "sequence_id": "10001"
    })"_json;
    return json.dump();
  });
}

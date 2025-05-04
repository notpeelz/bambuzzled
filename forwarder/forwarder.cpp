#include <type_traits>
#include <iostream>
#include <string>
#include <sstream>

#include <bambuzzled/log.hpp>
#include <bambuzzled/to_string.hpp>
#include <bambuzzled/networking/plugin.hpp>

#include "dyn_lib_loader.hpp"

namespace utils = bbzl::utils;
using bbzl::forwarder::dyn_lib_loader;

#if defined(_WIN32)

#include <windows.h>

BOOL WINAPI DllMain(
  HINSTANCE instance,
  DWORD reason,
  LPVOID reserved
) {
  (void)reserved;
  if (reason == DLL_PROCESS_ATTACH) {
    bbzl::log::set_level_filter(bbzl::log::log_level::TRACE);
  }
  return true;
}

#else

__attribute__((constructor))
static void ctor() {
  bbzl::log::set_level_filter(bbzl::log::log_level::TRACE);
}

#endif

namespace {

static std::string get_forward_path() {
  char const* s = std::getenv("BBZL_FORWARD");
  if (s == nullptr) {
    return "";
  }

  return s;
}

template<typename T>
T get_fn(std::string const& name) {
  static dyn_lib_loader* loader{};
  try {
    if (loader == nullptr) {
      auto forward_path = get_forward_path();
      if (forward_path.empty()) {
        return nullptr;
      }

      loader = new dyn_lib_loader(forward_path);
    }

    return loader->get_symbol<T>(name);
  } catch (std::exception& e) {
    bbzl::log::error_fmt("failed to get fn ptr for {}: {}", name, e.what());
    return nullptr;
  }
}

template<typename F, typename... Args>
void call_fn_void(char const* name, Args&&... args) {
  auto fn = get_fn<F>(name);
  if (fn) {
    return fn(std::forward<Args>(args)...);
  }
}

template<typename F, typename... Args>
inline auto call_fn(
  char const* name,
  typename std::invoke_result_t<F, Args...> fallback,
  Args&&... args
) -> typename std::invoke_result_t<F, Args...> {
  std::ostringstream oss;

  (([&](auto const& arg) {
    if (!oss.str().empty()) {
      oss << ", ";
    }
    oss << utils::to_string(arg);
  })(args), ...);

  auto fn = get_fn<F>(name);
  if (fn) {
    auto ret = fn(std::forward<Args>(args)...);
    bbzl::log::trace_fmt("{}({}) -> {}", name, oss.str(), utils::to_string(ret));
    return ret;
  }

  return fallback;
}

template<typename F>
inline auto create_wrapper_non_null(F orig, auto wrapper) {
  if (orig == nullptr) {
    return static_cast<F>(nullptr);
  } else {
    return static_cast<F>(wrapper);
  }
}

}

typedef bool (*func_check_debug_consistent)(bool is_debug);
bool bambu_network_check_debug_consistent(bool is_debug) {
  return call_fn<func_check_debug_consistent>(
    "bambu_network_check_debug_consistent",
    true,
    is_debug
  );
}

typedef std::string (*func_get_version)();
std::string bambu_network_get_version() {
  return call_fn<func_get_version>(
    "bambu_network_get_version",
    BAMBU_NETWORK_AGENT_VERSION
  );
}

typedef void* (*func_create_agent)(std::string log_dir);
void* bambu_network_create_agent(std::string log_dir) {
  return call_fn<func_create_agent>(
    "bambu_network_create_agent",
    nullptr,
    log_dir
  );
}

typedef int (*func_destroy_agent)(void* agent);
int bambu_network_destroy_agent(void* agent) {
  return call_fn<func_destroy_agent>(
    "bambu_network_destroy_agent",
    0,
    agent
  );
}

typedef int (*func_init_log)(void* agent);
int bambu_network_init_log(void* agent) {
  return call_fn<func_init_log>(
    "bambu_network_init_log",
    0,
    agent
  );
}

typedef int (*func_set_config_dir)(void* agent, std::string config_dir);
int bambu_network_set_config_dir(void* agent, std::string config_dir) {
  return call_fn<func_set_config_dir>(
    "bambu_network_set_config_dir",
    0,
    agent,
    config_dir
  );
}

typedef int (*func_set_cert_file)(void* agent, std::string folder, std::string filename);
int bambu_network_set_cert_file(void* agent, std::string folder, std::string filename) {
  return call_fn<func_set_cert_file>(
    "bambu_network_set_cert_file",
    0,
    agent,
    folder,
    filename
  );
}

typedef int (*func_set_country_code)(void* agent, std::string country_code);
int bambu_network_set_country_code(void* agent, std::string country_code) {
  return call_fn<func_set_country_code>(
    "bambu_network_set_country_code",
    0,
    agent,
    country_code
  );
}

typedef int (*func_start)(void* agent);
int bambu_network_start(void* agent) {
  return call_fn<func_start>(
    "bambu_network_start",
    0,
    agent
  );
}

typedef int (*func_set_on_ssdp_msg_fn)(void* agent, BBL::OnMsgArrivedFn fn);
int bambu_network_set_on_ssdp_msg_fn(void* agent, BBL::OnMsgArrivedFn fn) {
  return call_fn<func_set_on_ssdp_msg_fn>(
    "bambu_network_set_on_ssdp_msg_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](std::string dev_info_json_str) {
      auto scope = BBZL_TRACE_SCOPE(dev_info_json_str);
      return fn(dev_info_json_str);
    })
  );
}

typedef int (*func_set_on_user_login_fn)(void* agent, BBL::OnUserLoginFn fn);
int bambu_network_set_on_user_login_fn(void* agent, BBL::OnUserLoginFn fn) {
  return call_fn<func_set_on_user_login_fn>(
    "bambu_network_set_on_user_login_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](int online_login, bool login) {
      auto scope = BBZL_TRACE_SCOPE(online_login, login);
      return fn(online_login, login);
    })
  );
}

typedef int (*func_set_on_printer_connected_fn)(void* agent, BBL::OnPrinterConnectedFn fn);
int bambu_network_set_on_printer_connected_fn(void* agent, BBL::OnPrinterConnectedFn fn) {
  return call_fn<func_set_on_printer_connected_fn>(
    "bambu_network_set_on_printer_connected_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](std::string topic_str) {
      auto scope = BBZL_TRACE_SCOPE(topic_str);
      return fn(topic_str);
    })
  );
}

typedef int (*func_set_on_server_connected_fn)(void* agent, BBL::OnServerConnectedFn fn);
int bambu_network_set_on_server_connected_fn(void* agent, BBL::OnServerConnectedFn fn) {
  return call_fn<func_set_on_server_connected_fn>(
    "bambu_network_set_on_server_connected_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](int return_code, int reason_code) {
      auto scope = BBZL_TRACE_SCOPE(return_code, reason_code);
      return fn(return_code, reason_code);
    })
  );
}

typedef int (*func_set_on_http_error_fn)(void* agent, BBL::OnHttpErrorFn fn);
int bambu_network_set_on_http_error_fn(void* agent, BBL::OnHttpErrorFn fn) {
  return call_fn<func_set_on_http_error_fn>(
    "bambu_network_set_on_http_error_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](unsigned int http_code, std::string http_body) {
      auto scope = BBZL_TRACE_SCOPE(http_code, http_body);
      return fn(http_code, http_body);
    })
  );
}

typedef int (*func_set_get_country_code_fn)(void* agent, BBL::GetCountryCodeFn fn);
int bambu_network_set_get_country_code_fn(void* agent, BBL::GetCountryCodeFn fn) {
  return call_fn<func_set_get_country_code_fn>(
    "bambu_network_set_get_country_code_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn]() {
      auto scope = BBZL_TRACE_SCOPE();
      return fn();
    })
  );
}

typedef int (*func_set_on_subscribe_failure_fn)(void* agent, BBL::GetSubscribeFailureFn fn);
int bambu_network_set_on_subscribe_failure_fn(void* agent, BBL::GetSubscribeFailureFn fn) {
  return call_fn<func_set_on_subscribe_failure_fn>(
    "bambu_network_set_on_subscribe_failure_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](std::string topic_str) {
      auto scope = BBZL_TRACE_SCOPE(topic_str);
      return fn(topic_str);
    })
  );
}

typedef int (*func_set_on_message_fn)(void* agent, BBL::OnMessageFn fn);
int bambu_network_set_on_message_fn(void* agent, BBL::OnMessageFn fn) {
  return call_fn<func_set_on_message_fn>(
    "bambu_network_set_on_message_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](std::string dev_id, std::string msg) {
      auto scope = BBZL_TRACE_SCOPE(dev_id, msg);
      return fn(dev_id, msg);
    })
  );
}

typedef int (*func_set_on_user_message_fn)(void* agent, BBL::OnMessageFn fn);
int bambu_network_set_on_user_message_fn(void* agent, BBL::OnMessageFn fn) {
  return call_fn<func_set_on_user_message_fn>(
    "bambu_network_set_on_user_message_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](std::string dev_id, std::string msg) {
      auto scope = BBZL_TRACE_SCOPE(dev_id, msg);
      return fn(dev_id, msg);
    })
  );
}

typedef int (*func_set_on_local_connect_fn)(void* agent, BBL::OnLocalConnectedFn fn);
int bambu_network_set_on_local_connect_fn(void* agent, BBL::OnLocalConnectedFn fn) {
  return call_fn<func_set_on_local_connect_fn>(
    "bambu_network_set_on_local_connect_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](int status, std::string dev_id, std::string msg) {
      auto scope = BBZL_TRACE_SCOPE(status, dev_id, msg);
      return fn(status, dev_id, msg);
    })
  );
}

typedef int (*func_set_on_local_message_fn)(void* agent, BBL::OnMessageFn fn);
int bambu_network_set_on_local_message_fn(void* agent, BBL::OnMessageFn fn) {
  return call_fn<func_set_on_local_message_fn>(
    "bambu_network_set_on_local_message_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](std::string dev_id, std::string msg) {
      auto scope = BBZL_TRACE_SCOPE(dev_id, msg);
      return fn(dev_id, msg);
    })
  );
}

typedef int (*func_set_queue_on_main_fn)(void* agent, BBL::QueueOnMainFn fn);
int bambu_network_set_queue_on_main_fn(void* agent, BBL::QueueOnMainFn fn) {
  return call_fn<func_set_queue_on_main_fn>(
    "bambu_network_set_queue_on_main_fn",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](std::function<void()> f) {
      auto scope = BBZL_TRACE_SCOPE(f);
      return fn(f);
    })
  );
}

typedef int (*func_connect_server)(void* agent);
int bambu_network_connect_server(void* agent) {
  return call_fn<func_connect_server>(
    "bambu_network_connect_server",
    0,
    agent
  );
}

typedef bool (*func_is_server_connected)(void* agent);
bool bambu_network_is_server_connected(void* agent) {
  return call_fn<func_is_server_connected>(
    "bambu_network_is_server_connected",
    0,
    agent
  );
}

typedef int (*func_refresh_connection)(void* agent);
int bambu_network_refresh_connection(void* agent) {
  return call_fn<func_refresh_connection>(
    "bambu_network_refresh_connection",
    0,
    agent
  );
}

typedef int (*func_start_subscribe)(void* agent, std::string module);
int bambu_network_start_subscribe(void* agent, std::string module) {
  return call_fn<func_start_subscribe>(
    "bambu_network_start_subscribe",
    0,
    agent,
    module
  );
}

typedef int (*func_stop_subscribe)(void* agent, std::string module);
int bambu_network_stop_subscribe(void* agent, std::string module) {
  return call_fn<func_stop_subscribe>(
    "bambu_network_stop_subscribe",
    0,
    agent,
    module
  );
}

typedef int (*func_add_subscribe)(void* agent, std::vector<std::string> dev_list);
int bambu_network_add_subscribe(void* agent, std::vector<std::string> dev_list) {
  return call_fn<func_add_subscribe>(
    "bambu_network_add_subscribe",
    0,
    agent,
    dev_list
  );
}

typedef int (*func_del_subscribe)(void* agent, std::vector<std::string> dev_list);
int bambu_network_del_subscribe(void* agent, std::vector<std::string> dev_list) {
  return call_fn<func_del_subscribe>(
    "bambu_network_del_subscribe",
    0,
    agent,
    dev_list
  );
}

typedef void (*func_enable_multi_machine)(void* agent, bool enable);
void bambu_network_enable_multi_machine(void* agent, bool enable) {
  call_fn_void<func_enable_multi_machine>(
    "bambu_network_enable_multi_machine",
    agent,
    enable
  );
}

typedef int (*func_start_device_subscribe)(void* agent);
int bambu_network_start_device_subscribe(void* agent) {
  return call_fn<func_start_device_subscribe>(
    "bambu_network_start_device_subscribe",
    0,
    agent
  );
}

typedef int (*func_stop_device_subscribe)(void* agent);
int bambu_network_stop_device_subscribe(void* agent) {
  return call_fn<func_stop_device_subscribe>(
    "bambu_network_stop_device_subscribe",
    0,
    agent
  );
}

typedef int (*func_send_message)(void* agent, std::string dev_id, std::string json_str, int qos);
int bambu_network_send_message(void* agent, std::string dev_id, std::string json_str, int qos) {
  return call_fn<func_send_message>(
    "bambu_network_send_message",
    0,
    agent,
    dev_id,
    json_str,
    qos
  );
}

typedef int (*func_connect_printer)(void* agent, std::string dev_id, std::string dev_ip, std::string username, std::string password, bool use_ssl);
int bambu_network_connect_printer(void* agent, std::string dev_id, std::string dev_ip, std::string username, std::string password, bool use_ssl) {
  return call_fn<func_connect_printer>(
    "bambu_network_connect_printer",
    0,
    agent,
    dev_id,
    dev_ip,
    username,
    password,
    use_ssl
  );
}

typedef int (*func_disconnect_printer)(void* agent);
int bambu_network_disconnect_printer(void* agent) {
  return call_fn<func_disconnect_printer>(
    "bambu_network_disconnect_printer",
    0,
    agent
  );
}

typedef int (*func_send_message_to_printer)(void* agent, std::string dev_id, std::string json_str, int qos);
int bambu_network_send_message_to_printer(void* agent, std::string dev_id, std::string json_str, int qos) {
  return call_fn<func_send_message_to_printer>(
    "bambu_network_send_message_to_printer",
    0,
    agent,
    dev_id,
    json_str,
    qos
  );
}

typedef bool (*func_start_discovery)(void* agent, bool start, bool sending);
bool bambu_network_start_discovery(void* agent, bool start, bool sending) {
  return call_fn<func_start_discovery>(
    "bambu_network_start_discovery",
    0,
    agent,
    start,
    sending
  );
}

typedef int (*func_change_user)(void* agent, std::string user_info);
int bambu_network_change_user(void* agent, std::string user_info) {
  return call_fn<func_change_user>(
    "bambu_network_change_user",
    0,
    agent,
    user_info
  );
}

typedef bool (*func_is_user_login)(void* agent);
bool bambu_network_is_user_login(void* agent) {
  return call_fn<func_is_user_login>(
    "bambu_network_is_user_login",
    false,
    agent
  );
}

typedef int (*func_user_logout)(void* agent, bool request);
int bambu_network_user_logout(void* agent, bool request) {
  return call_fn<func_user_logout>(
    "bambu_network_user_logout",
    0,
    agent,
    request
  );
}

typedef std::string (*func_get_user_id)(void* agent);
std::string bambu_network_get_user_id(void* agent) {
  return call_fn<func_get_user_id>(
    "bambu_network_get_user_id",
    "",
    agent
  );
}

typedef std::string (*func_get_user_name)(void* agent);
std::string bambu_network_get_user_name(void* agent) {
  return call_fn<func_get_user_name>(
    "bambu_network_get_user_name",
    "",
    agent
  );
}

typedef std::string (*func_get_user_avatar)(void* agent);
std::string bambu_network_get_user_avatar(void* agent) {
  return call_fn<func_get_user_avatar>(
    "bambu_network_get_user_avatar",
    "",
    agent
  );
}

typedef std::string (*func_get_user_nickanme)(void* agent);
std::string bambu_network_get_user_nickanme(void* agent) {
  return call_fn<func_get_user_nickanme>(
    "bambu_network_get_user_nickanme",
    "",
    agent
  );
}

typedef std::string (*func_build_login_cmd)(void* agent);
std::string bambu_network_build_login_cmd(void* agent) {
  return call_fn<func_build_login_cmd>(
    "bambu_network_build_login_cmd",
    "",
    agent
  );
}

typedef std::string (*func_build_logout_cmd)(void* agent);
std::string bambu_network_build_logout_cmd(void* agent) {
  return call_fn<func_build_logout_cmd>(
    "bambu_network_build_logout_cmd",
    "",
    agent
  );
}

typedef std::string (*func_build_login_info)(void* agent);
std::string bambu_network_build_login_info(void* agent) {
  return call_fn<func_build_login_info>(
    "bambu_network_build_login_info",
    "",
    agent
  );
}

typedef int (*func_get_model_id_from_desgin_id)(void* agent, std::string& desgin_id, std::string& model_id);
int bambu_network_get_model_id_from_desgin_id(void* agent, std::string& desgin_id, std::string& model_id) {
  return call_fn<func_get_model_id_from_desgin_id>(
    "bambu_network_get_model_id_from_desgin_id",
    0,
    agent,
    desgin_id,
    model_id
  );
}

typedef int (*func_ping_bind)(void* agent, std::string ping_code);
int bambu_network_ping_bind(void* agent, std::string ping_code) {
  return call_fn<func_ping_bind>(
    "bambu_network_ping_bind",
    0,
    agent,
    ping_code
  );
}

typedef int (*func_bind_detect)(void* agent, std::string dev_ip, std::string sec_link, BBL::detectResult& detect);
int bambu_network_bind_detect(void* agent, std::string dev_ip, std::string sec_link, BBL::detectResult& detect) {
  return call_fn<func_bind_detect>(
    "bambu_network_bind_detect",
    0,
    agent,
    dev_ip,
    sec_link,
    detect
  );
}

typedef int (*func_set_server_callback)(void* agent, BBL::OnServerErrFn fn);
int bambu_network_set_server_callback(void* agent, BBL::OnServerErrFn fn) {
  return call_fn<func_set_server_callback>(
    "bambu_network_set_server_callback",
    0,
    agent,
    create_wrapper_non_null(fn, [fn](std::string url, int status) {
      auto scope = BBZL_TRACE_SCOPE(url, status);
      return fn(url, status);
    })
  );
}

typedef int (*func_bind)(void* agent, std::string dev_ip, std::string dev_id, std::string sec_link, std::string timezone, bool improved, BBL::OnUpdateStatusFn update_fn);
int bambu_network_bind(void* agent, std::string dev_ip, std::string dev_id, std::string sec_link, std::string timezone, bool improved, BBL::OnUpdateStatusFn update_fn) {
  return call_fn<func_bind>(
    "bambu_network_bind",
    0,
    agent,
    dev_ip,
    dev_id,
    sec_link,
    timezone,
    improved,
    update_fn
  );
}

typedef int (*func_unbind)(void* agent, std::string dev_id);
int bambu_network_unbind(void* agent, std::string dev_id) {
  return call_fn<func_unbind>(
    "bambu_network_unbind",
    0,
    agent,
    dev_id
  );
}

typedef std::string (*func_get_bambulab_host)(void* agent);
std::string bambu_network_get_bambulab_host(void* agent) {
  return call_fn<func_get_bambulab_host>(
    "bambu_network_get_bambulab_host",
    "",
    agent
  );
}

typedef std::string (*func_get_user_selected_machine)(void* agent);
std::string bambu_network_get_user_selected_machine(void* agent) {
  return call_fn<func_get_user_selected_machine>(
    "bambu_network_get_user_selected_machine",
    "",
    agent
  );
}

typedef int (*func_set_user_selected_machine)(void* agent, std::string dev_id);
int bambu_network_set_user_selected_machine(void* agent, std::string dev_id) {
  return call_fn<func_set_user_selected_machine>(
    "bambu_network_set_user_selected_machine",
    0,
    agent,
    dev_id
  );
}

typedef int (*func_start_print)(void* agent, BBL::PrintParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn, BBL::OnWaitFn wait_fn);
int bambu_network_start_print(void* agent, BBL::PrintParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn, BBL::OnWaitFn wait_fn) {
  return call_fn<func_start_print>(
    "bambu_network_start_print",
    0,
    agent,
    params,
    create_wrapper_non_null(update_fn, [update_fn](int status, int code, std::string msg) {
      auto scope = BBZL_TRACE_SCOPE_NAMED("update_fn", status, code, msg);
      return update_fn(status, code, msg);
    }),
    create_wrapper_non_null(cancel_fn, [cancel_fn]() {
      auto scope = BBZL_TRACE_SCOPE_NAMED("cancel_fn");
      return cancel_fn();
    }),
    create_wrapper_non_null(wait_fn, [wait_fn](int status, std::string job_info) {
      auto scope = BBZL_TRACE_SCOPE_NAMED("wait_fn", status, job_info);
      return wait_fn(status, job_info);
    })
  );
  return 0;
}

typedef int (*func_start_local_print_with_record)(void* agent, BBL::PrintParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn, BBL::OnWaitFn wait_fn);
int bambu_network_start_local_print_with_record(void* agent, BBL::PrintParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn, BBL::OnWaitFn wait_fn) {
  return call_fn<func_start_local_print_with_record>(
    "bambu_network_start_local_print_with_record",
    0,
    agent,
    params,
    create_wrapper_non_null(update_fn, [update_fn](int status, int code, std::string msg) {
      auto scope = BBZL_TRACE_SCOPE_NAMED("update_fn", status, code, msg);
      return update_fn(status, code, msg);
    }),
    create_wrapper_non_null(cancel_fn, [cancel_fn]() {
      auto scope = BBZL_TRACE_SCOPE_NAMED("cancel_fn");
      return cancel_fn();
    }),
    create_wrapper_non_null(wait_fn, [wait_fn](int status, std::string job_info) {
      auto scope = BBZL_TRACE_SCOPE_NAMED("wait_fn", status, job_info);
      return wait_fn(status, job_info);
    })
  );
  return 0;
}

typedef int (*func_start_send_gcode_to_sdcard)(void* agent, BBL::PrintParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn, BBL::OnWaitFn wait_fn);
int bambu_network_start_send_gcode_to_sdcard(void* agent, BBL::PrintParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn, BBL::OnWaitFn wait_fn) {
  return call_fn<func_start_send_gcode_to_sdcard>(
    "bambu_network_start_send_gcode_to_sdcard",
    0,
    agent,
    params,
    create_wrapper_non_null(update_fn, [update_fn](int status, int code, std::string msg) {
      auto scope = BBZL_TRACE_SCOPE_NAMED("update_fn", status, code, msg);
      return update_fn(status, code, msg);
    }),
    create_wrapper_non_null(cancel_fn, [cancel_fn]() {
      auto scope = BBZL_TRACE_SCOPE_NAMED("cancel_fn");
      return cancel_fn();
    }),
    create_wrapper_non_null(wait_fn, [wait_fn](int status, std::string job_info) {
      auto scope = BBZL_TRACE_SCOPE_NAMED("wait_fn", status, job_info);
      return wait_fn(status, job_info);
    })
  );
}

typedef int (*func_start_local_print)(void* agent, BBL::PrintParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn);
int bambu_network_start_local_print(void* agent, BBL::PrintParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn) {
  return call_fn<func_start_local_print>(
    "bambu_network_start_local_print",
    0,
    agent,
    params,
    create_wrapper_non_null(update_fn, [update_fn](int status, int code, std::string msg) {
      auto scope = BBZL_TRACE_SCOPE_NAMED("update_fn", status, code, msg);
      return update_fn(status, code, msg);
    }),
    create_wrapper_non_null(cancel_fn, [cancel_fn]() {
      auto scope = BBZL_TRACE_SCOPE_NAMED("cancel_fn");
      return cancel_fn();
    })
  );
  return 0;
}

typedef int (*func_start_sdcard_print)(void* agent, BBL::PrintParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn);
int bambu_network_start_sdcard_print(void* agent, BBL::PrintParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn) {
  return call_fn<func_start_sdcard_print>(
    "bambu_network_start_sdcard_print",
    0,
    agent,
    params,
    create_wrapper_non_null(update_fn, [update_fn](int status, int code, std::string msg) {
      auto scope = BBZL_TRACE_SCOPE_NAMED("update_fn", status, code, msg);
      return update_fn(status, code, msg);
    }),
    create_wrapper_non_null(cancel_fn, [cancel_fn]() {
      auto scope = BBZL_TRACE_SCOPE_NAMED("cancel_fn");
      return cancel_fn();
    })
  );
  return 0;
}

typedef int (*func_get_user_presets)(void* agent, std::map<std::string, std::map<std::string, std::string>>* user_presets);
int bambu_network_get_user_presets(void* agent, std::map<std::string, std::map<std::string, std::string>>* user_presets) {
  return call_fn<func_get_user_presets>(
    "bambu_network_get_user_presets",
    0,
    agent,
    user_presets
  );
}

typedef std::string (*func_request_setting_id)(void* agent, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code);
std::string bambu_network_request_setting_id(void* agent, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code) {
  return call_fn<func_request_setting_id>(
    "bambu_network_request_setting_id",
    "",
    agent,
    name,
    values_map,
    http_code
  );
}

typedef int (*func_put_setting)(void* agent, std::string setting_id, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code);
int bambu_network_put_setting(void* agent, std::string setting_id, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code) {
  return call_fn<func_put_setting>(
    "bambu_network_put_setting",
    0,
    agent,
    setting_id,
    name,
    values_map,
    http_code
  );
}

typedef int (*func_get_setting_list)(void* agent, std::string bundle_version, BBL::ProgressFn pro_fn, BBL::WasCancelledFn cancel_fn);
int bambu_network_get_setting_list(void* agent, std::string bundle_version, BBL::ProgressFn pro_fn, BBL::WasCancelledFn cancel_fn) {
  return call_fn<func_get_setting_list>(
    "bambu_network_get_setting_list",
    0,
    agent,
    bundle_version,
    pro_fn,
    cancel_fn
  );
}

typedef int (*func_get_setting_list2)(void* agent, std::string bundle_version, BBL::CheckFn chk_fn, BBL::ProgressFn pro_fn, BBL::WasCancelledFn cancel_fn);
int bambu_network_get_setting_list2(void* agent, std::string bundle_version, BBL::CheckFn chk_fn, BBL::ProgressFn pro_fn, BBL::WasCancelledFn cancel_fn) {
  return call_fn<func_get_setting_list2>(
    "bambu_network_get_setting_list2",
    0,
    agent,
    bundle_version,
    chk_fn,
    pro_fn,
    cancel_fn
  );
}

typedef int (*func_delete_setting)(void* agent, std::string setting_id);
int bambu_network_delete_setting(void* agent, std::string setting_id) {
  return call_fn<func_delete_setting>(
    "bambu_network_delete_setting",
    0,
    agent,
    setting_id
  );
}

typedef std::string (*func_get_studio_info_url)(void* agent);
std::string bambu_network_get_studio_info_url(void* agent) {
  return call_fn<func_get_studio_info_url>(
    "bambu_network_get_studio_info_url",
    "",
    agent
  );
}

typedef int (*func_set_extra_http_header)(void* agent, std::map<std::string, std::string> extra_headers);
int bambu_network_set_extra_http_header(void* agent, std::map<std::string, std::string> extra_headers) {
  return call_fn<func_set_extra_http_header>(
    "bambu_network_set_extra_http_header",
    0,
    agent,
    extra_headers
  );
}

typedef int (*func_get_my_message)(void* agent, int type, int after, int limit, unsigned int* http_code, std::string* http_body);
int bambu_network_get_my_message(void* agent, int type, int after, int limit, unsigned int* http_code, std::string* http_body) {
  return call_fn<func_get_my_message>(
    "bambu_network_get_my_message",
    0,
    agent,
    type,
    after,
    limit,
    http_code,
    http_body
  );
}

typedef int (*func_check_user_task_report)(void* agent, int* task_id, bool* printable);
int bambu_network_check_user_task_report(void* agent, int* task_id, bool* printable) {
  return call_fn<func_check_user_task_report>(
    "bambu_network_check_user_task_report",
    0,
    agent,
    task_id,
    printable
  );
}

typedef int (*func_get_user_print_info)(void* agent, unsigned int* http_code, std::string* http_body);
int bambu_network_get_user_print_info(void* agent, unsigned int* http_code, std::string* http_body) {
  return call_fn<func_get_user_print_info>(
    "bambu_network_get_user_print_info",
    0,
    agent,
    http_code,
    http_body
  );
}

typedef int (*func_get_user_tasks)(void* agent, BBL::TaskQueryParams params, std::string* http_body);
int bambu_network_get_user_tasks(void* agent, BBL::TaskQueryParams params, std::string* http_body) {
  return call_fn<func_get_user_tasks>(
    "bambu_network_get_user_tasks",
    0,
    agent,
    params,
    http_body
  );
}

typedef int (*func_get_printer_firmware)(void* agent, std::string dev_id, unsigned* http_code, std::string* http_body);
int bambu_network_get_printer_firmware(void* agent, std::string dev_id, unsigned* http_code, std::string* http_body) {
  return call_fn<func_get_printer_firmware>(
    "bambu_network_get_printer_firmware",
    0,
    agent,
    dev_id,
    http_code,
    http_body
  );
}

typedef int (*func_get_task_plate_index)(void* agent, std::string task_id, int* plate_index);
int bambu_network_get_task_plate_index(void* agent, std::string task_id, int* plate_index) {
  return call_fn<func_get_task_plate_index>(
    "bambu_network_get_task_plate_index",
    0,
    agent,
    task_id,
    plate_index
  );
}

typedef int (*func_get_user_info)(void* agent, int* identifier);
int bambu_network_get_user_info(void* agent, int* identifier) {
  return call_fn<func_get_user_info>(
    "bambu_network_get_user_info",
    0,
    agent,
    identifier
  );
}

typedef int (*func_request_bind_ticket)(void* agent, std::string* ticket);
int bambu_network_request_bind_ticket(void* agent, std::string* ticket) {
  return call_fn<func_request_bind_ticket>(
    "bambu_network_request_bind_ticket",
    0,
    agent,
    ticket
  );
}

typedef int (*func_get_subtask_info)(void* agent, std::string subtask_id, std::string* task_json, unsigned int* http_code, std::string* http_body);
int bambu_network_get_subtask_info(void* agent, std::string subtask_id, std::string* task_json, unsigned int* http_code, std::string* http_body) {
  return call_fn<func_get_subtask_info>(
    "bambu_network_get_subtask_info",
    0,
    agent,
    subtask_id,
    task_json,
    http_code,
    http_body
  );
}

typedef int (*func_get_slice_info)(void* agent, std::string project_id, std::string profile_id, int plate_index, std::string* slice_json);
int bambu_network_get_slice_info(void* agent, std::string project_id, std::string profile_id, int plate_index, std::string* slice_json) {
  return call_fn<func_get_slice_info>(
    "bambu_network_get_slice_info",
    0,
    agent,
    project_id,
    profile_id,
    plate_index,
    slice_json
  );
}

typedef int (*func_query_bind_status)(void* agent, std::vector<std::string> query_list, unsigned int* http_code, std::string* http_body);
int bambu_network_query_bind_status(void* agent, std::vector<std::string> query_list, unsigned int* http_code, std::string* http_body) {
  return call_fn<func_query_bind_status>(
    "bambu_network_query_bind_status",
    0,
    agent,
    query_list,
    http_code,
    http_body
  );
}

typedef int (*func_modify_printer_name)(void* agent, std::string dev_id, std::string dev_name);
int bambu_network_modify_printer_name(void* agent, std::string dev_id, std::string dev_name) {
  return call_fn<func_modify_printer_name>(
    "bambu_network_modify_printer_name",
    0,
    agent,
    dev_id,
    dev_name
  );
}

typedef int (*func_get_camera_url)(void* agent, std::string dev_id, std::function<void(std::string)> callback);
int bambu_network_get_camera_url(void* agent, std::string dev_id, std::function<void(std::string)> callback) {
  return call_fn<func_get_camera_url>(
    "bambu_network_get_camera_url",
    0,
    agent,
    dev_id,
    create_wrapper_non_null(callback, [callback](std::string url) {
      auto scope = BBZL_TRACE_SCOPE(url);
      return callback(url);
    })
  );
}

typedef int (*func_get_design_staffpick)(void* agent, int offset, int limit, std::function<void(std::string)> callback);
int bambu_network_get_design_staffpick(void* agent, int offset, int limit, std::function<void(std::string)> callback) {
  return call_fn<func_get_design_staffpick>(
    "bambu_network_get_design_staffpick",
    0,
    agent,
    offset,
    limit,
    callback
  );
}

typedef int (*func_start_publish)(void* agent, BBL::PublishParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn, std::string* out);
int bambu_network_start_publish(void* agent, BBL::PublishParams params, BBL::OnUpdateStatusFn update_fn, BBL::WasCancelledFn cancel_fn, std::string* out) {
  return call_fn<func_start_publish>(
    "bambu_network_start_publish",
    0,
    agent,
    params,
    update_fn,
    cancel_fn,
    out
  );
}

typedef int (*func_get_profile_3mf)(void* agent, Slic3r::BBLProfile* profile);
int bambu_network_get_profile_3mf(void* agent, Slic3r::BBLProfile* profile) {
  return call_fn<func_get_profile_3mf>(
    "bambu_network_get_profile_3mf",
    0,
    agent,
    profile
  );
}

typedef int (*func_get_model_publish_url)(void* agent, std::string* url);
int bambu_network_get_model_publish_url(void* agent, std::string* url) {
  return call_fn<func_get_model_publish_url>(
    "bambu_network_get_model_publish_url",
    0,
    agent,
    url
  );
}

typedef int (*func_get_subtask)(void* agent, Slic3r::BBLModelTask* task, Slic3r::OnGetSubTaskFn getsub_fn);
int bambu_network_get_subtask(void* agent, Slic3r::BBLModelTask* task, Slic3r::OnGetSubTaskFn getsub_fn) {
  return call_fn<func_get_subtask>(
    "bambu_network_get_subtask",
    0,
    agent,
    task,
    getsub_fn
  );
}

typedef int (*func_get_model_mall_home_url)(void* agent, std::string* url);
int bambu_network_get_model_mall_home_url(void* agent, std::string* url) {
  return call_fn<func_get_model_mall_home_url>(
    "bambu_network_get_model_mall_home_url",
    0,
    agent,
    url
  );
}

typedef int (*func_get_model_mall_detail_url)(void* agent, std::string* url, std::string id);
int bambu_network_get_model_mall_detail_url(void* agent, std::string* url, std::string id) {
  return call_fn<func_get_model_mall_detail_url>(
    "bambu_network_get_model_mall_detail_url",
    0,
    agent,
    url,
    id
  );
}

typedef int (*func_get_my_profile)(void* agent, std::string token, unsigned int* http_code, std::string* http_body);
int bambu_network_get_my_profile(void* agent, std::string token, unsigned int* http_code, std::string* http_body) {
  return call_fn<func_get_my_profile>(
    "bambu_network_get_my_profile",
    0,
    agent,
    token,
    http_code,
    http_body
  );
}

typedef int (*func_track_enable)(void* agent, bool enable);
int bambu_network_track_enable(void* agent, bool enable) {
  return call_fn<func_track_enable>(
    "bambu_network_track_enable",
    0,
    agent,
    enable
  );
}

typedef int (*func_track_remove_files)(void* agent);
int bambu_network_track_remove_files(void* agent) {
  return call_fn<func_track_remove_files>(
    "bambu_network_track_remove_files",
    0,
    agent
  );
}

typedef int (*func_track_event)(void* agent, std::string evt_key, std::string content);
int bambu_network_track_event(void* agent, std::string evt_key, std::string content) {
  return call_fn<func_track_event>(
    "bambu_network_track_event",
    0,
    agent,
    evt_key,
    content
  );
}

typedef int (*func_track_header)(void* agent, std::string header);
int bambu_network_track_header(void* agent, std::string header) {
  return call_fn<func_track_header>(
    "bambu_network_track_header",
    0,
    agent,
    header
  );
}

typedef int (*func_track_update_property)(void* agent, std::string name, std::string value, std::string type);
int bambu_network_track_update_property(void* agent, std::string name, std::string value, std::string type) {
  return call_fn<func_track_update_property>(
    "bambu_network_track_update_property",
    0,
    agent,
    name,
    value,
    type
  );
}

typedef int (*func_track_get_property)(void* agent, std::string name, std::string& value, std::string type);
int bambu_network_track_get_property(void* agent, std::string name, std::string& value, std::string type) {
  return call_fn<func_track_get_property>(
    "bambu_network_track_get_property",
    0,
    agent,
    name,
    value,
    type
  );
}

typedef int (*func_put_model_mall_rating_url)(void* agent, int rating_id, int score, std::string content, std::vector<std::string> images, unsigned int& http_code, std::string& http_error);
int bambu_network_put_model_mall_rating(void* agent, int rating_id, int score, std::string content, std::vector<std::string> images, unsigned int& http_code, std::string& http_error) {
  return call_fn<func_put_model_mall_rating_url>(
    "bambu_network_put_model_mall_rating",
    0,
    agent,
    rating_id,
    score,
    content,
    images,
    http_code,
    http_error
  );
}

typedef int (*func_get_oss_config)(void* agent, std::string& config, std::string country_code, unsigned int& http_code, std::string& http_error);
int bambu_network_get_oss_config(void* agent, std::string& config, std::string country_code, unsigned int& http_code, std::string& http_error) {
  return call_fn<func_get_oss_config>(
    "bambu_network_get_oss_config",
    0,
    agent,
    config,
    country_code,
    http_code,
    http_error
  );
}

typedef int (*func_put_rating_picture_oss)(void* agent, std::string& config, std::string& pic_oss_path, std::string model_id, int profile_id, unsigned int& http_code, std::string& http_error);
int bambu_network_put_rating_picture_oss(void* agent, std::string& config, std::string& pic_oss_path, std::string model_id, int profile_id, unsigned int& http_code, std::string& http_error) {
  return call_fn<func_put_rating_picture_oss>(
    "bambu_network_put_rating_picture_oss",
    0,
    agent,
    config,
    pic_oss_path,
    model_id,
    profile_id,
    http_code,
    http_error
  );
}

typedef int (*func_get_model_mall_rating_result)(void* agent, int job_id, std::string &rating_result, unsigned int &http_code, std::string &http_error);
int bambu_network_get_model_mall_rating(void* agent, int job_id, std::string& rating_result, unsigned int& http_code, std::string& http_error) {
  return call_fn<func_get_model_mall_rating_result>(
    "bambu_network_get_model_mall_rating",
    0,
    agent,
    job_id,
    rating_result,
    http_code,
    http_error
  );
}

typedef int (*func_get_mw_user_preference)(void* agent, std::function<void(std::string)> callback);
int bambu_network_get_mw_user_preference(void* agent, std::function<void(std::string)> callback) {
  return call_fn<func_get_mw_user_preference>(
    "bambu_network_get_mw_user_preference",
    0,
    agent,
    callback
  );
}

typedef int (*func_get_mw_user_4ulist)(void* agent, int seed, int limit, std::function<void(std::string)> callback);
int bambu_network_get_mw_user_4ulist(void* agent, int seed, int limit, std::function<void(std::string)> callback) {
  return call_fn<func_get_mw_user_4ulist>(
    "bambu_network_get_mw_user_4ulist",
    0,
    agent,
    seed,
    limit,
    callback
  );
}

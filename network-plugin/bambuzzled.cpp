#include <format>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <string>
#include <thread>
#include <cstdlib>
#include <utility>
#include <optional>
#include <exception>
#include <functional>
#include <string_view>
#include <nlohmann/json.hpp>
#include <bambuzzled/log.hpp>
#include <bambuzzled/curl.hpp>
#include <bambuzzled/socket.hpp>
#include <bambuzzled/try_wrapper.hpp>
#include <bambuzzled/networking/plugin.hpp>
#include <bambuzzled/networking/api/bambu_networking.hpp>
#include "network_agent.hpp"
#include "ftp.hpp"
#include "pairing.hpp"

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2ipdef.h>
#else
#include <sys/socket.h>
#endif

using json = nlohmann::json;
using network_agent = bbzl::network_agent;
using namespace std::string_view_literals;
using namespace bbzl::utils::try_wrapper;

bool bambu_network_check_debug_consistent(bool is_debug) {
  auto scope = BBZL_TRACE_SCOPE(is_debug);
  return true;
}

std::string bambu_network_get_version() {
  auto scope = BBZL_TRACE_SCOPE();
  return BAMBU_NETWORK_AGENT_VERSION;
}

void* bambu_network_create_agent(std::string log_dir) {
  auto scope = BBZL_TRACE_SCOPE(log_dir);
  return try_or(nullptr, []() {
    return new network_agent();
  });
}

int bambu_network_destroy_agent(void* agentp) {
  auto scope = BBZL_TRACE_SCOPE(agentp);
  try_([&]() {
    auto agent = static_cast<network_agent*>(agentp);
    delete agent;
  });
  return BAMBU_NETWORK_SUCCESS;
}

int bambu_network_start(void* agentp) {
  auto scope = BBZL_TRACE_SCOPE(agentp);
  auto agent = static_cast<network_agent*>(agentp);
  return BAMBU_NETWORK_SUCCESS;
}

bool bambu_network_start_discovery(void* agentp, bool start, bool sending) {
  auto scope = BBZL_TRACE_SCOPE(agentp, start, sending);
  return true;
}

std::string bambu_network_get_user_selected_machine(void* agentp) {
  auto scope = BBZL_TRACE_SCOPE(agentp);
  return try_or_default([&]() {
    auto agent = static_cast<network_agent*>(agentp);
    return agent->get_user_selected_machine();
  });
}

int bambu_network_set_user_selected_machine(void* agentp, std::string dev_id) {
  auto scope = BBZL_TRACE_SCOPE(agentp, dev_id);
  return try_or(1, [&]() {
    auto agent = static_cast<network_agent*>(agentp);
    agent->set_user_selected_machine(dev_id);
    return BAMBU_NETWORK_SUCCESS;
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
  auto scope = BBZL_TRACE_SCOPE(dev_id, dev_ip, username, "*PASSWORD*"sv);
  return try_or(BAMBU_NETWORK_ERR_CONNECT_FAILED, [&]() {
    auto agent = static_cast<network_agent*>(agentp);
    auto thread = std::thread(wrap_try([
      agent,
      dev_id = std::move(dev_id),
      dev_ip = std::move(dev_ip),
      username = std::move(username),
      password = std::move(password),
      use_ssl
    ]() {
      (void)agent->connect(dev_id, dev_ip, username, password, use_ssl);
    }));
    thread.detach();
    return BAMBU_NETWORK_SUCCESS;
  });
}

int bambu_network_send_message_to_printer(
  void* agentp,
  std::string dev_id,
  std::string json_str,
  int qos
) {
  auto scope = BBZL_TRACE_SCOPE(dev_id, dev_id, json_str);
  return try_or(BAMBU_NETWORK_ERR_SEND_MSG_FAILED, [&]() {
    auto agent = static_cast<network_agent*>(agentp);
    bool sent = false;
    std::string mqtt_err;
    try {
      sent = agent->send_message(
        dev_id,
        json_str,
        qos,
        std::ref(mqtt_err)
      );
    } catch (bbzl::network_client_not_found&) {
      bbzl::log::warn_fmt(
        "attempted to send message to a non-existent client: {}",
        dev_id
      );
    }

    if (!sent) {
      bbzl::log::error_fmt(
        "failed to send message to printer: {}",
        mqtt_err
      );
      return BAMBU_NETWORK_ERR_SEND_MSG_FAILED;
    }

    return BAMBU_NETWORK_SUCCESS;
  });
}

int bambu_network_disconnect_printer(void* agentp) {
  auto scope = BBZL_TRACE_SCOPE(agentp);
  return try_or(BAMBU_NETWORK_ERR_DISCONNECT_FAILED, [&]() {
    auto agent = static_cast<network_agent*>(agentp);
    agent->disconnect();
    return BAMBU_NETWORK_SUCCESS;
  });
}

int bambu_network_set_config_dir(void* agentp, std::string config_dir) {
  #if BBZL_TRACE_TRIVIAL
  auto scope = BBZL_TRACE_SCOPE(agentp, config_dir);
  #endif
  return try_or(1, [&]() {
    auto agent = static_cast<network_agent*>(agentp);
    agent->set_config_dir(std::move(config_dir));
    return BAMBU_NETWORK_SUCCESS;
  });
}

int bambu_network_set_on_printer_connected_fn(void* agentp, BBL::OnPrinterConnectedFn fn) {
  #if BBZL_TRACE_TRIVIAL
  auto scope = BBZL_TRACE_SCOPE(agentp, fn);
  #endif
  return try_or(1, [&]() {
    auto agent = static_cast<network_agent*>(agentp);
    agent->set_on_printer_connected_fn(std::move(fn));
    return BAMBU_NETWORK_SUCCESS;
  });
}

int bambu_network_set_on_local_connect_fn(void* agentp, BBL::OnLocalConnectedFn fn) {
  #if BBZL_TRACE_TRIVIAL
  auto scope = BBZL_TRACE_SCOPE(agentp, fn);
  #endif
  return try_or(1, [&]() {
    auto agent = static_cast<network_agent*>(agentp);
    agent->set_on_local_connected_fn(std::move(fn));
    return BAMBU_NETWORK_SUCCESS;
  });
}

int bambu_network_set_on_local_message_fn(void* agentp, BBL::OnMessageFn fn) {
  #if BBZL_TRACE_TRIVIAL
  auto scope = BBZL_TRACE_SCOPE(agentp, fn);
  #endif
  return try_or(1, [&]() {
    auto agent = static_cast<network_agent*>(agentp);
    agent->set_on_local_message_fn(std::move(fn));
    return BAMBU_NETWORK_SUCCESS;
  });
}

int bambu_network_set_queue_on_main_fn(void* agentp, BBL::QueueOnMainFn fn) {
  #if BBZL_TRACE_TRIVIAL
  auto scope = BBZL_TRACE_SCOPE(agentp, fn);
  #endif
  return try_or(1, [&]() {
    auto agent = static_cast<network_agent*>(agentp);
    agent->set_queue_on_main_fn(std::move(fn));
    return BAMBU_NETWORK_SUCCESS;
  });
}

std::string bambu_network_build_logout_cmd(void* agentp) {
  #if BBZL_TRACE_TRIVIAL
  auto scope = BBZL_TRACE_SCOPE(agentp);
  #endif

  return try_or_default([]() {
    auto json = R"({
      "command": "studio_useroffline",
      "sequence_id": "10001"
    })"_json;
    return json.dump();
  });
}

int bambu_network_bind_detect(
  void* agentp,
  std::string dev_ip,
  std::string sec_link,
  BBL::detectResult& result
) {
  enum return_code : int {
    RC_SUCCESS = 0,
    RC_CONNECT_FAILED = -1,
    RC_LOGIN_FAILED = -2,
    RC_INVALID_ADDRESS = -3
  };

  auto scope = BBZL_TRACE_SCOPE(agentp, dev_ip, sec_link, result);
  return try_or(RC_CONNECT_FAILED, [&]() -> int {
    auto agent = static_cast<network_agent*>(agentp);
    bool use_ssl = sec_link == "secure";
    bbzl::utils::net::net_port port = use_ssl ? 3002 : 3000;
    sockaddr_storage addr;
    try {
      addr = bbzl::utils::net::string_to_sockaddr(dev_ip, false);
    } catch (bbzl::utils::net::invalid_address&) {
      return RC_INVALID_ADDRESS;
    }

    std::function<std::string()> probe_binding_info;
    switch (addr.ss_family) {
      case AF_INET: {
        auto& s4 = *reinterpret_cast<sockaddr_in const*>(&addr);
        probe_binding_info = [&]() {
          return bbzl::pairing::probe_binding_info(s4.sin_addr, port, use_ssl);
        };
        break;
      }
      case AF_INET6: {
        auto& s6 = *reinterpret_cast<sockaddr_in6 const*>(&addr);
        probe_binding_info = [&]() {
          return bbzl::pairing::probe_binding_info(s6.sin6_addr, port, use_ssl);
        };
        break;
      }
      default: {
        std::abort();
        break;
      }
    }

    std::string s;
    try {
      s = probe_binding_info();
    } catch (std::exception& ex) {
      bbzl::log::error_fmt("failed to probe for binding info: {}", ex.what());
      return RC_CONNECT_FAILED;
    }
    result.result_msg = s;

    try {
      auto json = json::parse(s);
      auto& login = json["login"];
      result.command = login["command"];
      result.dev_id = login["id"];
      result.model_id = login["model"];
      result.dev_name = login["name"];
      result.version = login["version"];
      result.bind_state = login["bind"];
      result.connect_type = login["connect"];
    } catch (...) {
      bbzl::log::error_fmt("failed to parse json response: {}", s);
      return RC_CONNECT_FAILED;
    }

    return RC_SUCCESS;
  });
}

int bambu_network_start_print(
  void* agentp,
  BBL::PrintParams params,
  BBL::OnUpdateStatusFn update_fn,
  BBL::WasCancelledFn was_cancelled_fn,
  BBL::OnWaitFn wait_fn
) {
  auto scope = BBZL_TRACE_SCOPE(
    agentp,
    params,
    update_fn,
    was_cancelled_fn,
    wait_fn
  );
  return BAMBU_NETWORK_SUCCESS;
}

int bambu_network_start_local_print_with_record(
  void* agentp,
  BBL::PrintParams params,
  BBL::OnUpdateStatusFn update_fn,
  BBL::WasCancelledFn was_cancelled_fn,
  BBL::OnWaitFn wait_fn
) {
  auto scope = BBZL_TRACE_SCOPE(
    agentp,
    params,
    update_fn,
    was_cancelled_fn,
    wait_fn
  );
  return BAMBU_NETWORK_SUCCESS;
}

int bambu_network_start_send_gcode_to_sdcard(
  void* agentp,
  BBL::PrintParams params,
  BBL::OnUpdateStatusFn update_fn,
  BBL::WasCancelledFn was_cancelled_fn,
  BBL::OnWaitFn wait_fn
) {
  auto scope = BBZL_TRACE_SCOPE(
    agentp,
    params,
    update_fn,
    was_cancelled_fn,
    wait_fn
  );
  return try_or(1, [&]() {
    std::ifstream ifs;
    ifs.open(params.filename, std::ios::binary);
    if (!ifs.is_open() || ifs.fail()) {
      bbzl::log::error_fmt(
        "failed to open file: {}",
        params.filename
      );
      return BAMBU_NETWORK_ERR_PRINT_SG_UPLOAD_FTP_FAILED;
    }

    // TODO: callbacks, status updates, cancelling and error reporting
    std::string ftp_err;
    bool uploaded = bbzl::ftp::upload_file(
      std::format("ftps://{}/{}", params.dev_ip, params.project_name),
      params.username,
      params.password,
      ifs,
      std::nullopt,
      std::ref(ftp_err)
    );

    if (!uploaded) {
      bbzl::log::error_fmt(
        "failed to upload file: {}",
        ftp_err
      );
      return BAMBU_NETWORK_ERR_PRINT_SG_UPLOAD_FTP_FAILED;
    }

    return BAMBU_NETWORK_SUCCESS;
  });
}

int bambu_network_start_local_print(
  void* agentp,
  BBL::PrintParams params,
  BBL::OnUpdateStatusFn update_fn,
  BBL::WasCancelledFn was_cancelled_fn
) {
  auto scope = BBZL_TRACE_SCOPE(
    agentp,
    params,
    update_fn,
    was_cancelled_fn
  );
  return try_or(1, [&]() {
    auto agent = static_cast<network_agent*>(agentp);
    auto update_fn_ = update_fn == nullptr
      ? [](...) {}
      : update_fn;
    auto was_cancelled_fn_ = was_cancelled_fn == nullptr
      ? [](...) { return false; }
      : was_cancelled_fn;

    update_fn_(
      BBL::SendingPrintJobStage::PrintingStageUpload,
      BAMBU_NETWORK_SUCCESS,
      ""
    );

    std::ifstream ifs;
    ifs.open(params.filename, std::ios::binary);
    if (!ifs.is_open() || ifs.fail()) {
      bbzl::log::error_fmt(
        "failed to open file: {}",
        params.filename
      );
      return BAMBU_NETWORK_ERR_FILE_NOT_EXIST;
    }

    std::streampos ifs_size = 0;
    ifs_size = ifs.tellg();
    ifs.seekg(0, std::ios::end);
    ifs_size = ifs.tellg() - ifs_size;
    ifs.seekg(0, std::ios::beg);
    if (ifs.fail()) {
      update_fn_(
        BBL::SendingPrintJobStage::PrintingStageUpload,
        BAMBU_NETWORK_ERR_FILE_NOT_EXIST,
        "Failed to determine file size."
      );
      return BAMBU_NETWORK_ERR_FILE_NOT_EXIST;
    }

    if (ifs_size > 1 * 1024 * 1024 * 1024) {
      update_fn_(
        BBL::SendingPrintJobStage::PrintingStageUpload,
        BAMBU_NETWORK_ERR_PRINT_LP_FILE_OVER_SIZE,
        "The size of the uploaded file cannot exceed 1 GB."
      );
      return BAMBU_NETWORK_ERR_PRINT_LP_FILE_OVER_SIZE;
    }

    auto file_name = std::format("{}.gcode.3mf", params.project_name);
    auto url = std::format(
      "ftps://{}/{}",
      bbzl::utils::curl::urlencode(params.dev_ip),
      bbzl::utils::curl::urlencode(file_name)
    );

    bbzl::log::debug_fmt(
      "uploading file via FTP to {}: {}",
      params.dev_ip,
      params.filename
    );
    bbzl::log::trace_fmt("FTP url: {}", url);

    std::string ftp_err;
    bool uploaded = bbzl::ftp::upload_file(
      url,
      params.username,
      params.password,
      ifs,
      [&update_fn_](
        bbzl::ftp::file_offset total,
        bbzl::ftp::file_offset current
      ) {
        auto progress = static_cast<int>(
          100 * static_cast<double>(current) / total
        );
        update_fn_(
          BBL::SendingPrintJobStage::PrintingStageUpload,
          progress,
          ""
        );
      }
    );

    if (was_cancelled_fn()) {
      return BAMBU_NETWORK_ERR_CANCELED;
    }

    if (!uploaded) {
      bbzl::log::error_fmt(
        "failed to upload file: {}",
        ftp_err
      );

      update_fn_(
        BBL::SendingPrintJobStage::PrintingStageUpload,
        BAMBU_NETWORK_ERR_PRINT_LP_UPLOAD_FTP_FAILED,
        std::format("failed to upload file: {}", ftp_err)
      );

      return BAMBU_NETWORK_ERR_PRINT_LP_UPLOAD_FTP_FAILED;
    }

    update_fn_(
      BBL::SendingPrintJobStage::PrintingStageWaiting,
      BAMBU_NETWORK_ERR_PRINT_LP_UPLOAD_FTP_FAILED,
      ""
    );

    auto o = json::object();
    auto& p = o["print"];
    p = json::object({
      { "ams_mapping", json::array() }, // TODO
      { "bed_leveling", params.task_bed_leveling },
      { "bed_type", "auto" },
      { "command", "project_file" },
      { "file", file_name },
      { "flow_cali", params.task_flow_cali },
      { "layer_inspect", params.task_layer_inspect },
      { "md5", "" }, // TODO: compute md5 hash
      { "param", std::format("Metadata/plate_{}.gcode", params.plate_index) },
      { "profile_id", "0" },
      { "project_id", "0" },
      { "sequence_id", "50000" }, // TODO: "50000"? ++sequence_id
      { "subtask_id", "0" },
      { "subtask_name", params.project_name }, // ?
      { "task_id", "0" },
      { "timelapse", params.task_record_timelapse },
      { "use_ams", params.task_use_ams },
      { "vibration_cali", params.task_vibration_cali },
    });

    if (!params.task_bed_type.empty()) {
      p["bed_type"] = params.task_bed_type;
    }

    if (!params.ams_mapping.empty()) {
      try {
        p["ams_mapping"] = json::parse(params.ams_mapping);
      } catch (std::exception& ex) {
        update_fn_(
          BBL::SendingPrintJobStage::PrintingStageSending,
          BAMBU_NETWORK_ERR_PRINT_LP_PUBLISH_MSG_FAILED,
          std::format("failed to parse ams mapping: {}", ex.what())
        );
        return BAMBU_NETWORK_ERR_PRINT_LP_PUBLISH_MSG_FAILED;
      }
    }

    auto ftp_folder = params.ftp_folder;
    if (ftp_folder.length() > 1 && ftp_folder.back() == '/') {
      ftp_folder.pop_back();
    }

    if (params.print_type == "from_normal") {
      if (ftp_folder.empty()) {
        p["url"] = std::format(
          "file:///mnt/sdcard/{}",
          bbzl::utils::curl::urlencode(file_name)
        );
      } else {
        p["url"] = std::format(
          "file:///{}/{}",
          // XXX: this can contain slashes, so don't urlencode it
          ftp_folder,
          bbzl::utils::curl::urlencode(file_name)
        );
      }
    } else if (params.print_type == "from_sdcard_view") {
      p["url"] = std::format(
        "file:///{}",
        bbzl::utils::curl::urlencode(params.dst_file)
      );
    }

    auto msg = o.dump();
    bool sent = false;
    std::string mqtt_err;
    try {
      sent = agent->send_message(
        params.dev_id,
        msg,
        1,
        std::ref(mqtt_err)
      );
    } catch (bbzl::network_client_not_found&) {
      bbzl::log::warn_fmt(
        "attempted to send message to a non-existent client: {}",
        params.dev_id
      );
    }

    if (!sent) {
      bbzl::log::error_fmt(
        "failed to send message to printer: {}",
        mqtt_err
      );

      update_fn_(
        BBL::SendingPrintJobStage::PrintingStageSending,
        BAMBU_NETWORK_ERR_PRINT_LP_PUBLISH_MSG_FAILED,
        std::format("failed to send message: {}", mqtt_err)
      );

      return BAMBU_NETWORK_ERR_PRINT_LP_PUBLISH_MSG_FAILED;
    }

    update_fn_(
      BBL::SendingPrintJobStage::PrintingStageFinished,
      BAMBU_NETWORK_SUCCESS,
      ""
    );

    return BAMBU_NETWORK_SUCCESS;
  });
}

int bambu_network_start_sdcard_print(
  void* agentp,
  BBL::PrintParams params,
  BBL::OnUpdateStatusFn update_fn,
  BBL::WasCancelledFn was_cancelled_fn
) {
  auto scope = BBZL_TRACE_SCOPE(
    agentp,
    params,
    update_fn,
    was_cancelled_fn
  );
  return BAMBU_NETWORK_SUCCESS;
}

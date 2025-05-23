#include <format>
#include <string>
#include <thread>
#include <utility>
#include <exception>
#include <functional>
#include <filesystem>
#include <string_view>
#include <system_error>
#include <nlohmann/json.hpp>
#include <bambuzzled/log.hpp>
#include <bambuzzled/try_wrapper.hpp>
#include <bambuzzled/networking/plugin.hpp>
#include <bambuzzled/networking/api/bambu_networking.hpp>
#include "network_agent.hpp"
#include "ftp.hpp"
#include "pairing.hpp"

using json = nlohmann::json;
using network_agent = bbzl::network_agent;
using namespace std::string_view_literals;
using namespace bbzl::try_wrapper;

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
      agent->connect(dev_id, dev_ip, username, password, use_ssl);
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
    try {
      agent->send_message(dev_id, json_str, qos);
    } catch (bbzl::network_client_not_found&) {
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
  BBL::detectResult& detect
) {
  auto scope = BBZL_TRACE_SCOPE(agentp, dev_ip, sec_link, detect);
  return try_or(BAMBU_NETWORK_ERR_BIND_FAILED, [&]() {
    auto agent = static_cast<network_agent*>(agentp);
    bbzl::pairing::discover(dev_ip, sec_link == "secure", detect);
    return BAMBU_NETWORK_SUCCESS;
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
    // TODO: callbacks
    bbzl::ftp::upload_file(
      std::format("ftps://{}/{}", params.dev_ip, params.project_name),
      params.username,
      params.password,
      params.filename
    );
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
  auto agent = static_cast<network_agent*>(agentp);

  return try_or(1, [&]() {
    if (update_fn != nullptr) {
      update_fn(
        BBL::SendingPrintJobStage::PrintingStageUpload,
        BAMBU_NETWORK_SUCCESS,
        ""
      );
    }

    std::error_code ec;
    auto file_size = std::filesystem::file_size(params.filename, ec);
    if (ec.value() != 0) {
      return BAMBU_NETWORK_ERR_FILE_NOT_EXIST;
    }

    if (file_size > 1 * 1024 * 1024 * 1024) {
      if (update_fn != nullptr) {
        update_fn(
          BBL::SendingPrintJobStage::PrintingStageUpload,
          BAMBU_NETWORK_ERR_PRINT_LP_FILE_OVER_SIZE,
          "The size of the uploaded file cannot exceed 1 GB."
        );
      }
      return BAMBU_NETWORK_ERR_PRINT_LP_FILE_OVER_SIZE;
    }

    // TODO: urlencode
    auto file_name = std::format("{}.gcode.3mf", params.project_name);
    try {
      bbzl::ftp::upload_file(
        std::format("ftps://{}/{}", params.dev_ip, file_name),
        params.username,
        params.password,
        params.filename
      );
    } catch (std::exception& ex) {
      if (update_fn != nullptr) {
        update_fn(
          BBL::SendingPrintJobStage::PrintingStageUpload,
          BAMBU_NETWORK_ERR_PRINT_LP_UPLOAD_FTP_FAILED,
          ex.what()
        );
      }
      return BAMBU_NETWORK_ERR_PRINT_LP_UPLOAD_FTP_FAILED;
    }

    if (was_cancelled_fn()) {
      return BAMBU_NETWORK_ERR_CANCELED;
    }

    if (update_fn != nullptr) {
      update_fn(
        BBL::SendingPrintJobStage::PrintingStageWaiting,
        BAMBU_NETWORK_ERR_PRINT_LP_UPLOAD_FTP_FAILED,
        ""
      );
    }

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
        update_fn(
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
        p["url"] = std::format("file:///mnt/sdcard/{}", file_name);
      } else {
        p["url"] = std::format("file:///{}/{}", ftp_folder, file_name);
      }
    } else if (params.print_type == "from_sdcard_view") {
      p["url"] = std::format("file:///{}", params.dst_file);
    }

    auto s = o.dump();

    try {
      agent->send_message(params.dev_id, s, 1);
    } catch (std::exception& ex) {
      if (update_fn != nullptr) {
        update_fn(
          BBL::SendingPrintJobStage::PrintingStageSending,
          BAMBU_NETWORK_ERR_PRINT_LP_UPLOAD_FTP_FAILED,
          ex.what()
        );
      }
      return BAMBU_NETWORK_ERR_PRINT_LP_UPLOAD_FTP_FAILED;
    }

    update_fn(
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

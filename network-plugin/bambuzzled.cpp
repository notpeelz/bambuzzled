#include <format>
#include <string>
#include <utility>
#include <exception>
#include <functional>
#include <filesystem>
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
  (void)log_dir;
  return try_or(nullptr, []() {
    return new network_agent();
  });
}

int bambu_network_destroy_agent(void* agentp) {
  auto agent = static_cast<network_agent*>(agentp);
  try_([&]() {
    delete agent;
  });
  return 0;
}

int bambu_network_start(void* agentp) {
  auto agent = static_cast<network_agent*>(agentp);
  return try_or(1, [&agent]() {
    agent->start();
    return 0;
  });
}

bool bambu_network_start_discovery(void* agentp, bool start, bool sending) {
  auto scope = BBZL_TRACE_SCOPE(agentp, start, sending);
  return true;
}

std::string bambu_network_get_user_selected_machine(void* agentp) {
  auto scope = BBZL_TRACE_SCOPE(agentp);
  auto agent = static_cast<network_agent*>(agentp);
  return try_or_default([&]() {
    return agent->get_user_selected_machine();
  });
}

int bambu_network_set_user_selected_machine(void* agentp, std::string dev_id) {
  auto scope = BBZL_TRACE_SCOPE(agentp, dev_id);
  auto agent = static_cast<network_agent*>(agentp);
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
  auto agent = static_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->connect(dev_id, dev_ip, username, password, use_ssl);
    return 0;
  });
}

int bambu_network_send_message_to_printer(void* agentp, std::string dev_id, std::string json_str, int qos) {
  auto agent = static_cast<network_agent*>(agentp);
  return try_or(BAMBU_NETWORK_ERR_SEND_MSG_FAILED, [&]() {
    agent->send_message(dev_id, json_str, qos);
    return 0;
  });
}

int bambu_network_disconnect_printer(void* agentp) {
  (void)agentp;
  return 0;
}

int bambu_network_set_config_dir(void* agentp, std::string config_dir) {
  auto agent = static_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_config_dir(std::move(config_dir));
    return 0;
  });
}

int bambu_network_set_on_printer_connected_fn(void* agentp, BBL::OnPrinterConnectedFn fn) {
  auto agent = static_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_on_printer_connected_fn(std::move(fn));
    return 0;
  });
}

int bambu_network_set_on_server_connected_fn(void* agentp, BBL::OnServerConnectedFn fn) {
  auto agent = static_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_on_server_connected_fn(std::move(fn));
    return 0;
  });
}

int bambu_network_set_on_local_connect_fn(void* agentp, BBL::OnLocalConnectedFn fn) {
  auto agent = static_cast<network_agent*>(agentp);
  return try_or(1, [&]() {
    agent->set_on_local_connected_fn(std::move(fn));
    return 0;
  });
}

int bambu_network_set_on_local_message_fn(void* agentp, BBL::OnMessageFn fn) {
  auto agent = static_cast<network_agent*>(agentp);
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

int bambu_network_bind_detect(
  void* agentp,
  std::string dev_ip,
  std::string sec_link,
  BBL::detectResult& detect
) {
  auto scope = BBZL_TRACE_SCOPE(agentp, dev_ip, sec_link, detect);
  auto agent = static_cast<network_agent*>(agentp);

  return try_or(BAMBU_NETWORK_ERR_BIND_FAILED, [&]() {
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
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)was_cancelled_fn;
  (void)wait_fn;
  auto scope = BBZL_TRACE_SCOPE(agentp, params, update_fn, was_cancelled_fn, wait_fn);
  return 0;
}

int bambu_network_start_local_print_with_record(
  void* agentp,
  BBL::PrintParams params,
  BBL::OnUpdateStatusFn update_fn,
  BBL::WasCancelledFn was_cancelled_fn,
  BBL::OnWaitFn wait_fn
) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)was_cancelled_fn;
  (void)wait_fn;
  auto scope = BBZL_TRACE_SCOPE(agentp, params, update_fn, was_cancelled_fn, wait_fn);
  return 0;
}

int bambu_network_start_send_gcode_to_sdcard(
  void* agentp,
  BBL::PrintParams params,
  BBL::OnUpdateStatusFn update_fn,
  BBL::WasCancelledFn was_cancelled_fn,
  BBL::OnWaitFn wait_fn
) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)was_cancelled_fn;
  (void)wait_fn;

  // TODO: callbacks
  auto scope = BBZL_TRACE_SCOPE(agentp, params, update_fn, was_cancelled_fn, wait_fn);
  bbzl::ftp::upload_file(
    std::format("ftps://{}/{}", params.dev_ip, params.project_name),
    params.username,
    params.password,
    params.filename
  );

  return 0;
}

int bambu_network_start_local_print(
  void* agentp,
  BBL::PrintParams params,
  BBL::OnUpdateStatusFn update_fn,
  BBL::WasCancelledFn was_cancelled_fn
) {
  auto scope = BBZL_TRACE_SCOPE(agentp, params, update_fn, was_cancelled_fn);
  auto agent = static_cast<network_agent*>(agentp);

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
    return BAMBU_NETWORK_ERR_PRINT_WR_FILE_NOT_EXIST;
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

  return 0;
}

int bambu_network_start_sdcard_print(
  void* agentp,
  BBL::PrintParams params,
  BBL::OnUpdateStatusFn update_fn,
  BBL::WasCancelledFn was_cancelled_fn
) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)was_cancelled_fn;
  auto scope = BBZL_TRACE_SCOPE(agentp, params, update_fn, was_cancelled_fn);
  return 0;
}

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <bambuzzled/log.hpp>
#include <bambuzzled/networking/plugin.hpp>
#include <bambuzzled/networking/api/project_task.hpp>
#include <bambuzzled/networking/api/bambu_networking.hpp>

int bambu_network_init_log(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return 0;
}

int bambu_network_set_cert_file(
  void* agentp,
  std::string folder,
  std::string filename
) {
  BBZL_LOG_STUB(agentp, folder, filename);
  return 0;
}

int bambu_network_set_country_code(
  void* agentp,
  std::string country_code
) {
  BBZL_LOG_STUB(agentp, country_code);
  return 0;
}

int bambu_network_set_on_ssdp_msg_fn(
  void* agentp,
  BBL::OnMsgArrivedFn fn
) {
  #if BBZL_TRACE_TRIVIAL
  BBZL_LOG_STUB(agentp, fn);
  #endif
  return 0;
}

int bambu_network_set_on_user_login_fn(
  void* agentp,
  BBL::OnUserLoginFn fn
) {
  #if BBZL_TRACE_TRIVIAL
  BBZL_LOG_STUB(agentp, fn);
  #endif
  return 0;
}

int bambu_network_set_on_server_connected_fn(
  void* agentp,
  BBL::OnServerConnectedFn fn
) {
  #if BBZL_TRACE_TRIVIAL
  auto scope = BBZL_TRACE_SCOPE(agentp, fn);
  #endif
  return 0;
}

int bambu_network_set_on_http_error_fn(
  void* agentp,
  BBL::OnHttpErrorFn fn
) {
  #if BBZL_TRACE_TRIVIAL
  BBZL_LOG_STUB(agentp, fn);
  #endif
  return 0;
}

int bambu_network_set_get_country_code_fn(
  void* agentp,
  BBL::GetCountryCodeFn fn
) {
  #if BBZL_TRACE_TRIVIAL
  BBZL_LOG_STUB(agentp, fn);
  #endif
  return 0;
}

int bambu_network_set_on_subscribe_failure_fn(
  void* agentp,
  BBL::GetSubscribeFailureFn fn
) {
  #if BBZL_TRACE_TRIVIAL
  BBZL_LOG_STUB(agentp, fn);
  #endif
  return 0;
}

int bambu_network_set_on_message_fn(
  void* agentp,
  BBL::OnMessageFn fn
) {
  #if BBZL_TRACE_TRIVIAL
  BBZL_LOG_STUB(agentp, fn);
  #endif
  return 0;
}

int bambu_network_set_on_user_message_fn(
  void* agentp,
  BBL::OnMessageFn fn
) {
  #if BBZL_TRACE_TRIVIAL
  BBZL_LOG_STUB(agentp, fn);
  #endif
  return 0;
}

int bambu_network_connect_server(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return 0;
}

bool bambu_network_is_server_connected(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return 0;
}

int bambu_network_refresh_connection(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return 0;
}

int bambu_network_start_subscribe(void* agentp, std::string module) {
  BBZL_LOG_STUB(agentp, module);
  return 0;
}

int bambu_network_stop_subscribe(void* agentp, std::string module) {
  BBZL_LOG_STUB(agentp, module);
  return 0;
}

int bambu_network_add_subscribe(
  void* agentp,
  std::vector<std::string> dev_list
) {
  BBZL_LOG_STUB(agentp, dev_list);
  return 0;
}

int bambu_network_del_subscribe(
  void* agentp,
  std::vector<std::string> dev_list
) {
  BBZL_LOG_STUB(agentp, dev_list);
  return 0;
}

void bambu_network_enable_multi_machine(void* agentp, bool enable) {
  BBZL_LOG_STUB(agentp, enable);
}

int bambu_network_start_device_subscribe(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return 0;
}

int bambu_network_stop_device_subscribe(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return 0;
}

int bambu_network_send_message(
  void* agentp,
  std::string dev_id,
  std::string json_str,
  int qos
) {
  BBZL_LOG_STUB(agentp, dev_id, json_str, qos);
  return 0;
}

int bambu_network_change_user(void* agentp, std::string user_info) {
  BBZL_LOG_STUB(agentp, user_info);
  return 0;
}

bool bambu_network_is_user_login(void* agentp) {
  #if BBZL_TRACE_TRIVIAL
  BBZL_LOG_STUB(agentp);
  #endif
  return false;
}

int bambu_network_user_logout(void* agentp, bool request) {
  #if BBZL_TRACE_TRIVIAL
  BBZL_LOG_STUB(agentp, request);
  #endif
  return 0;
}

std::string bambu_network_get_user_id(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return "";
}

std::string bambu_network_get_user_name(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return "";
}

std::string bambu_network_get_user_avatar(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return "";
}

std::string bambu_network_get_user_nickanme(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return "";
}

std::string bambu_network_build_login_cmd(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return "";
}

std::string bambu_network_build_login_info(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return "";
}

int bambu_network_get_model_id_from_desgin_id(
  void* agentp,
  std::string& desgin_id,
  std::string& model_id
) {
  BBZL_LOG_STUB(agentp, desgin_id, model_id);
  return 0;
}

int bambu_network_ping_bind(void* agentp, std::string ping_code) {
  BBZL_LOG_STUB(agentp, ping_code);
  return 0;
}

int bambu_network_set_server_callback(
  void* agentp,
  BBL::OnServerErrFn fn
) {
  #if BBZL_TRACE_TRIVIAL
  BBZL_LOG_STUB(agentp, fn);
  #endif
  return 0;
}

int bambu_network_bind(
  void* agentp,
  std::string dev_ip,
  std::string dev_id,
  std::string sec_link,
  std::string timezone,
  bool improved,
  BBL::OnUpdateStatusFn update_fn
) {
  BBZL_LOG_STUB(agentp, dev_ip, dev_id, sec_link, timezone, improved, update_fn);
  return 0;
}

int bambu_network_unbind(void* agentp, std::string dev_id) {
  BBZL_LOG_STUB(agentp, dev_id);
  return 0;
}

std::string bambu_network_get_bambulab_host(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return "";
}

int bambu_network_get_user_presets(
  void* agentp,
  std::map<std::string, std::map<std::string, std::string>>* user_presets
) {
  BBZL_LOG_STUB(agentp, user_presets);
  return 0;
}

std::string bambu_network_request_setting_id(
  void* agentp,
  std::string name,
  std::map<std::string, std::string>* values_map,
  unsigned int* http_code
) {
  BBZL_LOG_STUB(agentp, name, values_map, http_code);
  return "";
}

int bambu_network_put_setting(
  void* agentp,
  std::string setting_id,
  std::string name,
  std::map<std::string, std::string>* values_map,
  unsigned int* http_code
) {
  BBZL_LOG_STUB(agentp, setting_id, name, values_map, http_code);
  return 0;
}

int bambu_network_get_setting_list(
  void* agentp,
  std::string bundle_version,
  BBL::ProgressFn pro_fn,
  BBL::WasCancelledFn was_cancelled_fn
) {
  BBZL_LOG_STUB(agentp, bundle_version, pro_fn, was_cancelled_fn);
  return 0;
}

int bambu_network_get_setting_list2(
  void* agentp,
  std::string bundle_version,
  BBL::CheckFn chk_fn,
  BBL::ProgressFn pro_fn,
  BBL::WasCancelledFn was_cancelled_fn
) {
  BBZL_LOG_STUB(
    agentp,
    bundle_version,
    chk_fn,
    pro_fn,
    was_cancelled_fn
  );
  return 0;
}

int bambu_network_delete_setting(
  void* agentp,
  std::string setting_id
) {
  BBZL_LOG_STUB(agentp, setting_id);
  return 0;
}

std::string bambu_network_get_studio_info_url(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return "";
}

int bambu_network_set_extra_http_header(
  void* agentp,
  std::map<std::string, std::string> extra_headers
) {
  BBZL_LOG_STUB(agentp, extra_headers);
  return 0;
}

int bambu_network_get_my_message(
  void* agentp,
  int type,
  int after,
  int limit,
  unsigned int* http_code,
  std::string* http_body
) {
  BBZL_LOG_STUB(agentp, type, after, limit, http_code, http_body);
  return 0;
}

int bambu_network_check_user_task_report(
  void* agentp,
  int* task_id,
  bool* printable
) {
  BBZL_LOG_STUB(agentp, task_id, printable);
  return 0;
}

int bambu_network_get_user_print_info(
  void* agentp,
  unsigned int* http_code,
  std::string* http_body
) {
  BBZL_LOG_STUB(agentp, http_code, http_body);
  return 0;
}

int bambu_network_get_user_tasks(
  void* agentp,
  BBL::TaskQueryParams params,
  std::string* http_body
) {
  BBZL_LOG_STUB(agentp, params, http_body);
  return 0;
}

int bambu_network_get_printer_firmware(
  void* agentp,
  std::string dev_id,
  unsigned* http_code,
  std::string* http_body
) {
  BBZL_LOG_STUB(agentp, dev_id, http_code, http_body);
  return 0;
}

int bambu_network_get_task_plate_index(
  void* agentp,
  std::string task_id,
  int* plate_index
) {
  BBZL_LOG_STUB(agentp, task_id, plate_index);
  return 0;
}

int bambu_network_get_user_info(void* agentp, int* identifier) {
  BBZL_LOG_STUB(agentp, identifier);
  return 0;
}

int bambu_network_request_bind_ticket(
  void* agentp,
  std::string* ticket
) {
  BBZL_LOG_STUB(agentp, ticket);
  return 0;
}

int bambu_network_get_subtask_info(
  void* agentp,
  std::string subtask_id,
  std::string* task_json,
  unsigned int* http_code,
  std::string* http_body
) {
  BBZL_LOG_STUB(agentp, subtask_id, task_json, http_code, http_body);
  return 0;
}

int bambu_network_get_slice_info(
  void* agentp,
  std::string project_id,
  std::string profile_id,
  int plate_index,
  std::string* slice_json
) {
  BBZL_LOG_STUB(agentp, project_id, profile_id, plate_index, slice_json);
  return 0;
}

int bambu_network_query_bind_status(
  void* agentp,
  std::vector<std::string> query_list,
  unsigned int* http_code,
  std::string* http_body
) {
  BBZL_LOG_STUB(agentp, query_list, http_code, http_body);
  return 0;
}

int bambu_network_modify_printer_name(
  void* agentp,
  std::string dev_id,
  std::string dev_name
) {
  BBZL_LOG_STUB(agentp, dev_id, dev_name);
  return 0;
}

int bambu_network_get_camera_url(
  void* agentp,
  std::string dev_id,
  std::function<void(std::string)> callback
) {
  BBZL_LOG_STUB(agentp, dev_id, callback);
  return 0;
}

int bambu_network_get_design_staffpick(
  void* agentp,
  int offset,
  int limit,
  std::function<void(std::string)> callback
) {
  BBZL_LOG_STUB(agentp, offset, limit, callback);
  return 0;
}

int bambu_network_start_publish(
  void* agentp,
  BBL::PublishParams params,
  BBL::OnUpdateStatusFn update_fn,
  BBL::WasCancelledFn was_cancelled_fn,
  std::string* out
) {
  BBZL_LOG_STUB(agentp, params, update_fn, was_cancelled_fn, out);
  return 0;
}

int bambu_network_get_profile_3mf(
  void* agentp,
  Slic3r::BBLProfile* profile
) {
  BBZL_LOG_STUB(agentp, profile);
  return 0;
}

int bambu_network_get_model_publish_url(
  void* agentp,
  std::string* url
) {
  BBZL_LOG_STUB(agentp, url);
  return 0;
}

int bambu_network_get_subtask(
  void* agentp, Slic3r::BBLModelTask* task,
  Slic3r::OnGetSubTaskFn getsub_fn
) {
  BBZL_LOG_STUB(agentp, task, getsub_fn);
  return 0;
}

int bambu_network_get_model_mall_home_url(
  void* agentp,
  std::string* url
) {
  BBZL_LOG_STUB(agentp, url);
  return 0;
}

int bambu_network_get_model_mall_detail_url(
  void* agentp,
  std::string* url,
  std::string id
) {
  BBZL_LOG_STUB(agentp, url, id);
  return 0;
}

int bambu_network_get_my_profile(
  void* agentp,
  std::string token,
  unsigned int* http_code,
  std::string* http_body
) {
  BBZL_LOG_STUB(agentp, token, http_code, http_body);
  return 0;
}

int bambu_network_track_enable(void* agentp, bool enable) {
  BBZL_LOG_STUB(agentp, enable);
  return 0;
}

int bambu_network_track_remove_files(void* agentp) {
  BBZL_LOG_STUB(agentp);
  return 0;
}

int bambu_network_track_event(
  void* agentp,
  std::string evt_key,
  std::string content
) {
  BBZL_LOG_STUB(agentp, evt_key, content);
  return 0;
}

int bambu_network_track_header(void* agentp, std::string header) {
  BBZL_LOG_STUB(agentp, header);
  return 0;
}

int bambu_network_track_update_property(
  void* agentp,
  std::string name,
  std::string value,
  std::string type
) {
  BBZL_LOG_STUB(agentp, name, value, type);
  return 0;
}

int bambu_network_track_get_property(
  void* agentp,
  std::string name,
  std::string& value,
  std::string type
) {
  BBZL_LOG_STUB(agentp, name, value, type);
  return 0;
}

int bambu_network_put_model_mall_rating(
  void* agentp,
  int rating_id,
  int score,
  std::string content,
  std::vector<std::string> images,
  unsigned int& http_code,
  std::string& http_error
) {
  BBZL_LOG_STUB(agentp, rating_id, score, content, images, http_code, http_error);
  return 0;
}

int bambu_network_get_oss_config(
  void* agentp,
  std::string& config,
  std::string country_code,
  unsigned int& http_code,
  std::string& http_error
) {
  BBZL_LOG_STUB(agentp, config, country_code, http_code, http_error);
  return 0;
}

int bambu_network_put_rating_picture_oss(
  void* agentp,
  std::string& config,
  std::string& pic_oss_path,
  std::string model_id,
  int profile_id,
  unsigned int& http_code,
  std::string& http_error
) {
  BBZL_LOG_STUB(agentp, config, pic_oss_path, model_id, profile_id, http_code, http_error);
  return 0;
}

int bambu_network_get_model_mall_rating(
  void* agentp,
  int job_id,
  std::string& rating_result,
  unsigned int& http_code,
  std::string& http_error
) {
  BBZL_LOG_STUB(agentp, job_id, rating_result, http_code, http_error);
  return 0;
}

int bambu_network_get_mw_user_preference(
  void* agentp,
  std::function<void(std::string)> callback
) {
  BBZL_LOG_STUB(agentp, callback);
  return 0;
}

int bambu_network_get_mw_user_4ulist(
  void* agentp,
  int seed,
  int limit,
  std::function<void(std::string)> callback
) {
  BBZL_LOG_STUB(agentp, seed, limit, callback);
  return 0;
}

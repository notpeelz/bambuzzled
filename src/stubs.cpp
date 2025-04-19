#include <type_traits>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>

#include "bambuzzled.hpp"

typedef int (*func_init_log)(void* agentp);
int bambu_network_init_log(void* agentp) {
  (void)agentp;
  return 0;
}

typedef int (*func_set_cert_file)(void* agentp, std::string folder, std::string filename);
int bambu_network_set_cert_file(void* agentp, std::string&& folder, std::string&& filename) {
  (void)agentp;
  (void)folder;
  (void)filename;
  return 0;
}

typedef int (*func_set_country_code)(void* agentp, std::string country_code);
int bambu_network_set_country_code(void* agentp, std::string&& country_code) {
  (void)agentp;
  (void)country_code;
  return 0;
}

typedef int (*func_start)(void* agentp);
int bambu_network_start(void* agentp) {
  (void)agentp;
  return 0;
}

typedef int (*func_set_on_ssdp_msg_fn)(void* agentp, OnMsgArrivedFn fn);
int bambu_network_set_on_ssdp_msg_fn(void* agentp, OnMsgArrivedFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_user_login_fn)(void* agentp, OnUserLoginFn fn);
int bambu_network_set_on_user_login_fn(void* agentp, OnUserLoginFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_printer_connected_fn)(void* agentp, OnPrinterConnectedFn fn);
int bambu_network_set_on_printer_connected_fn(void* agentp, OnPrinterConnectedFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_server_connected_fn)(void* agentp, OnServerConnectedFn fn);
int bambu_network_set_on_server_connected_fn(void* agentp, OnServerConnectedFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_http_error_fn)(void* agentp, OnHttpErrorFn fn);
int bambu_network_set_on_http_error_fn(void* agentp, OnHttpErrorFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_set_get_country_code_fn)(void* agentp, GetCountryCodeFn fn);
int bambu_network_set_get_country_code_fn(void* agentp, GetCountryCodeFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_subscribe_failure_fn)(void* agentp, GetSubscribeFailureFn fn);
int bambu_network_set_on_subscribe_failure_fn(void* agentp, GetSubscribeFailureFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_message_fn)(void* agentp, OnMessageFn fn);
int bambu_network_set_on_message_fn(void* agentp, OnMessageFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_user_message_fn)(void* agentp, OnMessageFn fn);
int bambu_network_set_on_user_message_fn(void* agentp, OnMessageFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_local_connect_fn)(void* agentp, OnLocalConnectedFn fn);
int bambu_network_set_on_local_connect_fn(void* agentp, OnLocalConnectedFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_local_message_fn)(void* agentp, OnMessageFn fn);
int bambu_network_set_on_local_message_fn(void* agentp, OnMessageFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_set_queue_on_main_fn)(void* agentp, QueueOnMainFn fn);
int bambu_network_set_queue_on_main_fn(void* agentp, QueueOnMainFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_connect_server)(void* agentp);
int bambu_network_connect_server(void* agentp) {
  (void)agentp;
  return 0;
}

typedef bool (*func_is_server_connected)(void* agentp);
bool bambu_network_is_server_connected(void* agentp) {
  (void)agentp;
  return 0;
}

typedef int (*func_refresh_connection)(void* agentp);
int bambu_network_refresh_connection(void* agentp) {
  (void)agentp;
  return 0;
}

typedef int (*func_start_subscribe)(void* agentp, std::string module);
int bambu_network_start_subscribe(void* agentp, std::string&& module) {
  (void)agentp;
  (void)module;
  return 0;
}

typedef int (*func_stop_subscribe)(void* agentp, std::string module);
int bambu_network_stop_subscribe(void* agentp, std::string&& module) {
  (void)agentp;
  (void)module;
  return 0;
}

typedef int (*func_add_subscribe)(void* agentp, std::vector<std::string> dev_list);
int bambu_network_add_subscribe(void* agentp, std::vector<std::string>&& dev_list) {
  (void)agentp;
  (void)dev_list;
  return 0;
}

typedef int (*func_del_subscribe)(void* agentp, std::vector<std::string> dev_list);
int bambu_network_del_subscribe(void* agentp, std::vector<std::string>&& dev_list) {
  (void)agentp;
  (void)dev_list;
  return 0;
}

typedef void (*func_enable_multi_machine)(void* agentp, bool enable);
void bambu_network_enable_multi_machine(void* agentp, bool enable) {
  (void)agentp;
  (void)enable;
}

typedef int (*func_start_device_subscribe)(void* agentp);
int bambu_network_start_device_subscribe(void* agentp) {
  (void)agentp;
  return 0;
}

typedef int (*func_stop_device_subscribe)(void* agentp);
int bambu_network_stop_device_subscribe(void* agentp) {
  (void)agentp;
  return 0;
}

typedef int (*func_send_message)(void* agentp, std::string dev_id, std::string json_str, int qos);
int bambu_network_send_message(void* agentp, std::string&& dev_id, std::string&& json_str, int qos) {
  (void)agentp;
  (void)dev_id;
  (void)json_str;
  (void)qos;
  return 0;
}

typedef int (*func_connect_printer)(void* agentp, std::string dev_id, std::string dev_ip, std::string username, std::string password, bool use_ssl);
int bambu_network_connect_printer(void* agentp, std::string&& dev_id, std::string&& dev_ip, std::string&& username, std::string&& password, bool use_ssl) {
  (void)agentp;
  (void)dev_id;
  (void)dev_ip;
  (void)username;
  (void)password;
  (void)use_ssl;
  return 0;
}

typedef int (*func_disconnect_printer)(void* agentp);
int bambu_network_disconnect_printer(void* agentp) {
  (void)agentp;
  return 0;
}

typedef int (*func_send_message_to_printer)(void* agentp, std::string dev_id, std::string json_str, int qos);
int bambu_network_send_message_to_printer(void* agentp, std::string&& dev_id, std::string&& json_str, int qos) {
  (void)agentp;
  (void)dev_id;
  (void)json_str;
  (void)qos;
  return 0;
}

typedef bool (*func_start_discovery)(void* agentp, bool start, bool sending);
bool bambu_network_start_discovery(void* agentp, bool start, bool sending) {
  (void)agentp;
  (void)start;
  (void)sending;
  return 0;
}

typedef int (*func_change_user)(void* agentp, std::string user_info);
int bambu_network_change_user(void* agentp, std::string&& user_info) {
  (void)agentp;
  (void)user_info;
  return 0;
}

typedef bool (*func_is_user_login)(void* agentp);
bool bambu_network_is_user_login(void* agentp) {
  (void)agentp;
  return false;
}

typedef int (*func_user_logout)(void* agentp, bool request);
int bambu_network_user_logout(void* agentp, bool request) {
  (void)agentp;
  (void)request;
  return 0;
}

typedef std::string (*func_get_user_id)(void* agentp);
std::string bambu_network_get_user_id(void* agentp) {
  (void)agentp;
  return "";
}

typedef std::string (*func_get_user_name)(void* agentp);
std::string bambu_network_get_user_name(void* agentp) {
  (void)agentp;
  return "";
}

typedef std::string (*func_get_user_avatar)(void* agentp);
std::string bambu_network_get_user_avatar(void* agentp) {
  (void)agentp;
  return "";
}

typedef std::string (*func_get_user_nickanme)(void* agentp);
std::string bambu_network_get_user_nickanme(void* agentp) {
  (void)agentp;
  return "";
}

typedef std::string (*func_build_login_cmd)(void* agentp);
std::string bambu_network_build_login_cmd(void* agentp) {
  (void)agentp;
  return "";
}

typedef std::string (*func_build_login_info)(void* agentp);
std::string bambu_network_build_login_info(void* agentp) {
  (void)agentp;
  return "";
}

typedef int (*func_get_model_id_from_desgin_id)(void* agentp, std::string& desgin_id, std::string& model_id);
int bambu_network_get_model_id_from_desgin_id(void* agentp, std::string&& desgin_id, std::string&& model_id) {
  (void)agentp;
  (void)desgin_id;
  (void)model_id;
  return 0;
}

typedef int (*func_ping_bind)(void* agentp, std::string ping_code);
int bambu_network_ping_bind(void* agentp, std::string&& ping_code) {
  (void)agentp;
  (void)ping_code;
  return 0;
}

typedef int (*func_bind_detect)(void* agentp, std::string dev_ip, std::string sec_link, detectResult& detect);
int bambu_network_bind_detect(void* agentp, std::string&& dev_ip, std::string&& sec_link, detectResult&& detect) {
  (void)agentp;
  (void)dev_ip;
  (void)sec_link;
  (void)detect;
  return 0;
}

typedef int (*func_set_server_callback)(void* agentp, OnServerErrFn fn);
int bambu_network_set_server_callback(void* agentp, OnServerErrFn&& fn) {
  (void)agentp;
  (void)fn;
  return 0;
}

typedef int (*func_bind)(void* agentp, std::string dev_ip, std::string dev_id, std::string sec_link, std::string timezone, bool improved, OnUpdateStatusFn update_fn);
int bambu_network_bind(void* agentp, std::string&& dev_ip, std::string&& dev_id, std::string&& sec_link, std::string&& timezone, bool improved, OnUpdateStatusFn&& update_fn) {
  (void)agentp;
  (void)dev_ip;
  (void)dev_id;
  (void)sec_link;
  (void)timezone;
  (void)improved;
  (void)update_fn;
  return 0;
}

typedef int (*func_unbind)(void* agentp, std::string dev_id);
int bambu_network_unbind(void* agentp, std::string&& dev_id) {
  (void)agentp;
  (void)dev_id;
  return 0;
}

typedef std::string (*func_get_bambulab_host)(void* agentp);
std::string bambu_network_get_bambulab_host(void* agentp) {
  (void)agentp;
  return "";
}

typedef std::string (*func_get_user_selected_machine)(void* agentp);
std::string bambu_network_get_user_selected_machine(void* agentp) {
  (void)agentp;
  return "";
}

typedef int (*func_set_user_selected_machine)(void* agentp, std::string dev_id);
int bambu_network_set_user_selected_machine(void* agentp, std::string&& dev_id) {
  (void)agentp;
  (void)dev_id;
  return 0;
}

typedef int (*func_start_print)(void* agentp, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
int bambu_network_start_print(void* agentp, PrintParams&& params, OnUpdateStatusFn&& update_fn, WasCancelledFn&& cancel_fn, OnWaitFn&& wait_fn) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  (void)wait_fn;
  return 0;
}

typedef int (*func_start_local_print_with_record)(void* agentp, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
int bambu_network_start_local_print_with_record(void* agentp, PrintParams&& params, OnUpdateStatusFn&& update_fn, WasCancelledFn&& cancel_fn, OnWaitFn&& wait_fn) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  (void)wait_fn;
  return 0;
}

typedef int (*func_start_send_gcode_to_sdcard)(void* agentp, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
int bambu_network_start_send_gcode_to_sdcard(void* agentp, PrintParams&& params, OnUpdateStatusFn&& update_fn, WasCancelledFn&& cancel_fn, OnWaitFn&& wait_fn) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  (void)wait_fn;
  return 0;
}

typedef int (*func_start_local_print)(void* agentp, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn);
int bambu_network_start_local_print(void* agentp, PrintParams&& params, OnUpdateStatusFn&& update_fn, WasCancelledFn&& cancel_fn) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  return 0;
}

typedef int (*func_start_sdcard_print)(void* agentp, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn);
int bambu_network_start_sdcard_print(void* agentp, PrintParams&& params, OnUpdateStatusFn&& update_fn, WasCancelledFn&& cancel_fn) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  return 0;
}

typedef int (*func_get_user_presets)(void* agentp, std::map<std::string, std::map<std::string, std::string>>* user_presets);
int bambu_network_get_user_presets(void* agentp, std::map<std::string, std::map<std::string, std::string>>* user_presets) {
  (void)agentp;
  (void)user_presets;
  return 0;
}

typedef std::string (*func_request_setting_id)(void* agentp, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code);
std::string bambu_network_request_setting_id(void* agentp, std::string&& name, std::map<std::string, std::string>* values_map, unsigned int* http_code) {
  (void)agentp;
  (void)name;
  (void)values_map;
  (void)http_code;
  return "";
}

typedef int (*func_put_setting)(void* agentp, std::string setting_id, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code);
int bambu_network_put_setting(void* agentp, std::string&& setting_id, std::string&& name, std::map<std::string, std::string>* values_map, unsigned int* http_code) {
  (void)agentp;
  (void)setting_id;
  (void)name;
  (void)values_map;
  (void)http_code;
  return 0;
}

typedef int (*func_get_setting_list)(void* agentp, std::string bundle_version, ProgressFn pro_fn, WasCancelledFn cancel_fn);
int bambu_network_get_setting_list(void* agentp, std::string&& bundle_version, ProgressFn&& pro_fn, WasCancelledFn&& cancel_fn) {
  (void)agentp;
  (void)bundle_version;
  (void)pro_fn;
  (void)cancel_fn;
  return 0;
}

typedef int (*func_get_setting_list2)(void* agentp, std::string bundle_version, CheckFn chk_fn, ProgressFn pro_fn, WasCancelledFn cancel_fn);
int bambu_network_get_setting_list2(void* agentp, std::string&& bundle_version, CheckFn&& chk_fn, ProgressFn&& pro_fn, WasCancelledFn&& cancel_fn) {
  (void)agentp;
  (void)bundle_version;
  (void)chk_fn;
  (void)pro_fn;
  (void)cancel_fn;
  return 0;
}

typedef int (*func_delete_setting)(void* agentp, std::string setting_id);
int bambu_network_delete_setting(void* agentp, std::string&& setting_id) {
  (void)agentp;
  (void)setting_id;
  return 0;
}

typedef std::string (*func_get_studio_info_url)(void* agentp);
std::string bambu_network_get_studio_info_url(void* agentp) {
  (void)agentp;
  return "";
}

typedef int (*func_set_extra_http_header)(void* agentp, std::map<std::string, std::string> extra_headers);
int bambu_network_set_extra_http_header(void* agentp, std::map<std::string, std::string>&& extra_headers) {
  (void)agentp;
  (void)extra_headers;
  return 0;
}

typedef int (*func_get_my_message)(void* agentp, int type, int after, int limit, unsigned int* http_code, std::string* http_body);
int bambu_network_get_my_message(void* agentp, int type, int after, int limit, unsigned int* http_code, std::string* http_body) {
  (void)agentp;
  (void)type;
  (void)after;
  (void)limit;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_check_user_task_report)(void* agentp, int* task_id, bool* printable);
int bambu_network_check_user_task_report(void* agentp, int* task_id, bool* printable) {
  (void)agentp;
  (void)task_id;
  (void)printable;
  return 0;
}

typedef int (*func_get_user_print_info)(void* agentp, unsigned int* http_code, std::string* http_body);
int bambu_network_get_user_print_info(void* agentp, unsigned int* http_code, std::string* http_body) {
  (void)agentp;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_get_user_tasks)(void* agentp, TaskQueryParams params, std::string* http_body);
int bambu_network_get_user_tasks(void* agentp, TaskQueryParams&& params, std::string* http_body) {
  (void)agentp;
  (void)params;
  (void)http_body;
  return 0;
}

typedef int (*func_get_printer_firmware)(void* agentp, std::string dev_id, unsigned* http_code, std::string* http_body);
int bambu_network_get_printer_firmware(void* agentp, std::string&& dev_id, unsigned* http_code, std::string* http_body) {
  (void)agentp;
  (void)dev_id;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_get_task_plate_index)(void* agentp, std::string task_id, int* plate_index);
int bambu_network_get_task_plate_index(void* agentp, std::string&& task_id, int* plate_index) {
  (void)agentp;
  (void)task_id;
  (void)plate_index;
  return 0;
}

typedef int (*func_get_user_info)(void* agentp, int* identifier);
int bambu_network_get_user_info(void* agentp, int* identifier) {
  (void)agentp;
  (void)identifier;
  return 0;
}

typedef int (*func_request_bind_ticket)(void* agentp, std::string* ticket);
int bambu_network_request_bind_ticket(void* agentp, std::string* ticket) {
  (void)agentp;
  (void)ticket;
  return 0;
}

typedef int (*func_get_subtask_info)(void* agentp, std::string subtask_id, std::string* task_json, unsigned int* http_code, std::string* http_body);
int bambu_network_get_subtask_info(void* agentp, std::string&& subtask_id, std::string* task_json, unsigned int* http_code, std::string* http_body) {
  (void)agentp;
  (void)subtask_id;
  (void)task_json;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_get_slice_info)(void* agentp, std::string project_id, std::string profile_id, int plate_index, std::string* slice_json);
int bambu_network_get_slice_info(void* agentp, std::string&& project_id, std::string&& profile_id, int plate_index, std::string* slice_json) {
  (void)agentp;
  (void)project_id;
  (void)profile_id;
  (void)plate_index;
  (void)slice_json;
  return 0;
}

typedef int (*func_query_bind_status)(void* agentp, std::vector<std::string> query_list, unsigned int* http_code, std::string* http_body);
int bambu_network_query_bind_status(void* agentp, std::vector<std::string>&& query_list, unsigned int* http_code, std::string* http_body) {
  (void)agentp;
  (void)query_list;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_modify_printer_name)(void* agentp, std::string dev_id, std::string dev_name);
int bambu_network_modify_printer_name(void* agentp, std::string&& dev_id, std::string&& dev_name) {
  (void)agentp;
  (void)dev_id;
  (void)dev_name;
  return 0;
}

typedef int (*func_get_camera_url)(void* agentp, std::string dev_id, std::function<void(std::string)> callback);
int bambu_network_get_camera_url(void* agentp, std::string&& dev_id, std::function<void(std::string)>&& callback) {
  (void)agentp;
  (void)dev_id;
  (void)callback;
  return 0;
}

typedef int (*func_get_design_staffpick)(void* agentp, int offset, int limit, std::function<void(std::string)> callback);
int bambu_network_get_design_staffpick(void* agentp, int offset, int limit, std::function<void(std::string)>&& callback) {
  (void)agentp;
  (void)offset;
  (void)limit;
  (void)callback;
  return 0;
}

typedef int (*func_start_publish)(void* agentp, PublishParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, std::string* out);
int bambu_network_start_publish(void* agentp, PublishParams&& params, OnUpdateStatusFn&& update_fn, WasCancelledFn&& cancel_fn, std::string* out) {
  (void)agentp;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  (void)out;
  return 0;
}

typedef int (*func_get_profile_3mf)(void* agentp, BBLProfile* profile);
int bambu_network_get_profile_3mf(void* agentp, BBLProfile* profile) {
  (void)agentp;
  (void)profile;
  return 0;
}

typedef int (*func_get_model_publish_url)(void* agentp, std::string* url);
int bambu_network_get_model_publish_url(void* agentp, std::string* url) {
  (void)agentp;
  (void)url;
  return 0;
}

typedef int (*func_get_subtask)(void* agentp, BBLModelTask* task, OnGetSubTaskFn getsub_fn);
int bambu_network_get_subtask(void* agentp, BBLModelTask* task, OnGetSubTaskFn&& getsub_fn) {
  (void)agentp;
  (void)task;
  (void)getsub_fn;
  return 0;
}

typedef int (*func_get_model_mall_home_url)(void* agentp, std::string* url);
int bambu_network_get_model_mall_home_url(void* agentp, std::string* url) {
  (void)agentp;
  (void)url;
  return 0;
}

typedef int (*func_get_model_mall_detail_url)(void* agentp, std::string* url, std::string id);
int bambu_network_get_model_mall_detail_url(void* agentp, std::string* url, std::string&& id) {
  (void)agentp;
  (void)url;
  (void)id;
  return 0;
}

typedef int (*func_get_my_profile)(void* agentp, std::string token, unsigned int* http_code, std::string* http_body);
int bambu_network_get_my_profile(void* agentp, std::string&& token, unsigned int* http_code, std::string* http_body) {
  (void)agentp;
  (void)token;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_track_enable)(void* agentp, bool enable);
int bambu_network_track_enable(void* agentp, bool enable) {
  (void)agentp;
  (void)enable;
  return 0;
}

typedef int (*func_track_remove_files)(void* agentp);
int bambu_network_track_remove_files(void* agentp) {
  (void)agentp;
  return 0;
}

typedef int (*func_track_event)(void* agentp, std::string evt_key, std::string content);
int bambu_network_track_event(void* agentp, std::string&& evt_key, std::string&& content) {
  (void)agentp;
  (void)evt_key;
  (void)content;
  return 0;
}

typedef int (*func_track_header)(void* agentp, std::string header);
int bambu_network_track_header(void* agentp, std::string&& header) {
  (void)agentp;
  (void)header;
  return 0;
}

typedef int (*func_track_update_property)(void* agentp, std::string name, std::string value, std::string type);
int bambu_network_track_update_property(void* agentp, std::string&& name, std::string&& value, std::string&&type) {
  (void)agentp;
  (void)name;
  (void)value;
  (void)type;
  return 0;
}

typedef int (*func_track_get_property)(void* agentp, std::string name, std::string& value, std::string type);
int bambu_network_track_get_property(void* agentp, std::string&& name, std::string& value, std::string&& type) {
  (void)agentp;
  (void)name;
  (void)value;
  (void)type;
  return 0;
}

typedef int (*func_put_model_mall_rating_url)(void* agentp, int rating_id, int score, std::string content, std::vector<std::string> images, unsigned int& http_code, std::string& http_error);
int bambu_network_put_model_mall_rating(void* agentp, int rating_id, int score, std::string&& content, std::vector<std::string>&& images, unsigned int& http_code, std::string& http_error) {
  (void)agentp;
  (void)rating_id;
  (void)score;
  (void)content;
  (void)images;
  (void)http_code;
  (void)http_error;
  return 0;
}

typedef int (*func_get_oss_config)(void* agentp, std::string& config, std::string country_code, unsigned int& http_code, std::string& http_error);
int bambu_network_get_oss_config(void* agentp, std::string& config, std::string&& country_code, unsigned int& http_code, std::string&& http_error) {
  (void)agentp;
  (void)config;
  (void)country_code;
  (void)http_code;
  (void)http_error;
  return 0;
}

typedef int (*func_put_rating_picture_oss)(void* agentp, std::string& config, std::string& pic_oss_path, std::string model_id, int profile_id, unsigned int& http_code, std::string& http_error);
int bambu_network_put_rating_picture_oss(void* agentp, std::string& config, std::string& pic_oss_path, std::string&& model_id, int profile_id, unsigned int& http_code, std::string& http_error) {
  (void)agentp;
  (void)config;
  (void)pic_oss_path;
  (void)model_id;
  (void)profile_id;
  (void)http_code;
  (void)http_error;
  return 0;
}

typedef int (*func_get_model_mall_rating_result)(void* agentp, int job_id, std::string &rating_result, unsigned int &http_code, std::string &http_error);
int bambu_network_get_model_mall_rating(void* agentp, int job_id, std::string& rating_result, unsigned int& http_code, std::string& http_error) {
  (void)agentp;
  (void)job_id;
  (void)rating_result;
  (void)http_code;
  (void)http_error;
  return 0;
}

typedef int (*func_get_mw_user_preference)(void* agentp, std::function<void(std::string)> callback);
int bambu_network_get_mw_user_preference(void* agentp, std::function<void(std::string)>&& callback) {
  (void)agentp;
  (void)callback;
  return 0;
}

typedef int (*func_get_mw_user_4ulist)(void* agentp, int seed, int limit, std::function<void(std::string)> callback);
int bambu_network_get_mw_user_4ulist(void* agentp, int seed, int limit, std::function<void(std::string)>&& callback) {
  (void)agentp;
  (void)seed;
  (void)limit;
  (void)callback;
  return 0;
}

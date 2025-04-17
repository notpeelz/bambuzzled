#include <type_traits>
#include <iostream>
#include <string>
#include <sstream>

#include <bambuzzled/api/bambu_networking.hpp>
#include <bambuzzled/api/project_task.hpp>

#include "bambuzzled.hpp"

typedef bool (*func_check_debug_consistent)(bool is_debug);
bool bambu_network_check_debug_consistent(bool is_debug) {
  (void)is_debug;
  return true;
}

typedef std::string (*func_get_version)(void);
std::string bambu_network_get_version(void) {
  return "01.10.01.50";
}

typedef void* (*func_create_agent)(std::string log_dir);
void* bambu_network_create_agent(std::string log_dir) {
  (void)log_dir;
  return nullptr;
}

typedef int (*func_destroy_agent)(void* agent);
int bambu_network_destroy_agent(void* agent) {
  (void)agent;
  return 0;
}

typedef int (*func_init_log)(void* agent);
int bambu_network_init_log(void* agent) {
  (void)agent;
  return 0;
}

typedef int (*func_set_config_dir)(void *agent, std::string config_dir);
int bambu_network_set_config_dir(void *agent, std::string config_dir) {
  (void)agent;
  (void)config_dir;
  return 0;
}

typedef int (*func_set_cert_file)(void *agent, std::string folder, std::string filename);
int bambu_network_set_cert_file(void *agent, std::string folder, std::string filename) {
  (void)agent;
  (void)folder;
  (void)filename;
  return 0;
}

typedef int (*func_set_country_code)(void *agent, std::string country_code);
int bambu_network_set_country_code(void *agent, std::string country_code) {
  (void)agent;
  (void)country_code;
  return 0;
}

typedef int (*func_start)(void *agent);
int bambu_network_start(void *agent) {
  (void)agent;
  return 0;
}

typedef int (*func_set_on_ssdp_msg_fn)(void *agent, OnMsgArrivedFn fn);
int bambu_network_set_on_ssdp_msg_fn(void *agent, OnMsgArrivedFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_user_login_fn)(void *agent, OnUserLoginFn fn);
int bambu_network_set_on_user_login_fn(void *agent, OnUserLoginFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_printer_connected_fn)(void *agent, OnPrinterConnectedFn fn);
int bambu_network_set_on_printer_connected_fn(void *agent, OnPrinterConnectedFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_server_connected_fn)(void *agent, OnServerConnectedFn fn);
int bambu_network_set_on_server_connected_fn(void *agent, OnServerConnectedFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_http_error_fn)(void *agent, OnHttpErrorFn fn);
int bambu_network_set_on_http_error_fn(void *agent, OnHttpErrorFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_set_get_country_code_fn)(void *agent, GetCountryCodeFn fn);
int bambu_network_set_get_country_code_fn(void *agent, GetCountryCodeFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_subscribe_failure_fn)(void *agent, GetSubscribeFailureFn fn);
int bambu_network_set_on_subscribe_failure_fn(void *agent, GetSubscribeFailureFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_message_fn)(void *agent, OnMessageFn fn);
int bambu_network_set_on_message_fn(void *agent, OnMessageFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_user_message_fn)(void *agent, OnMessageFn fn);
int bambu_network_set_on_user_message_fn(void *agent, OnMessageFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_local_connect_fn)(void *agent, OnLocalConnectedFn fn);
int bambu_network_set_on_local_connect_fn(void *agent, OnLocalConnectedFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_set_on_local_message_fn)(void *agent, OnMessageFn fn);
int bambu_network_set_on_local_message_fn(void *agent, OnMessageFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_set_queue_on_main_fn)(void *agent, QueueOnMainFn fn);
int bambu_network_set_queue_on_main_fn(void *agent, QueueOnMainFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_connect_server)(void *agent);
int bambu_network_connect_server(void *agent) {
  (void)agent;
  return 0;
}

typedef bool (*func_is_server_connected)(void *agent);
bool bambu_network_is_server_connected(void *agent) {
  (void)agent;
  return 0;
}

typedef int (*func_refresh_connection)(void *agent);
int bambu_network_refresh_connection(void *agent) {
  (void)agent;
  return 0;
}

typedef int (*func_start_subscribe)(void *agent, std::string module);
int bambu_network_start_subscribe(void *agent, std::string module) {
  (void)agent;
  (void)module;
  return 0;
}

typedef int (*func_stop_subscribe)(void *agent, std::string module);
int bambu_network_stop_subscribe(void *agent, std::string module) {
  (void)agent;
  (void)module;
  return 0;
}

typedef int (*func_add_subscribe)(void *agent, std::vector<std::string> dev_list);
int bambu_network_add_subscribe(void *agent, std::vector<std::string> dev_list) {
  (void)agent;
  (void)dev_list;
  return 0;
}

typedef int (*func_del_subscribe)(void *agent, std::vector<std::string> dev_list);
int bambu_network_del_subscribe(void *agent, std::vector<std::string> dev_list) {
  (void)agent;
  (void)dev_list;
  return 0;
}

typedef void (*func_enable_multi_machine)(void *agent, bool enable);
void bambu_network_enable_multi_machine(void *agent, bool enable) {
  (void)agent;
  (void)enable;
}

typedef int (*func_start_device_subscribe)(void* agent);
int bambu_network_start_device_subscribe(void* agent) {
  (void)agent;
  return 0;
}

typedef int (*func_stop_device_subscribe)(void* agent);
int bambu_network_stop_device_subscribe(void* agent) {
  (void)agent;
  return 0;
}

typedef int (*func_send_message)(void *agent, std::string dev_id, std::string json_str, int qos);
int bambu_network_send_message(void *agent, std::string dev_id, std::string json_str, int qos) {
  (void)agent;
  (void)dev_id;
  (void)json_str;
  (void)qos;
  return 0;
}

typedef int (*func_connect_printer)(void *agent, std::string dev_id, std::string dev_ip, std::string username, std::string password, bool use_ssl);
int bambu_network_connect_printer(void *agent, std::string dev_id, std::string dev_ip, std::string username, std::string password, bool use_ssl) {
  (void)agent;
  (void)dev_id;
  (void)dev_ip;
  (void)username;
  (void)password;
  (void)use_ssl;
  return 0;
}

typedef int (*func_disconnect_printer)(void *agent);
int bambu_network_disconnect_printer(void *agent) {
  (void)agent;
  return 0;
}

typedef int (*func_send_message_to_printer)(void *agent, std::string dev_id, std::string json_str, int qos);
int bambu_network_send_message_to_printer(void *agent, std::string dev_id, std::string json_str, int qos) {
  (void)agent;
  (void)dev_id;
  (void)json_str;
  (void)qos;
  return 0;
}

typedef bool (*func_start_discovery)(void *agent, bool start, bool sending);
bool bambu_network_start_discovery(void *agent, bool start, bool sending) {
  (void)agent;
  (void)start;
  (void)sending;
  return 0;
}

typedef int (*func_change_user)(void *agent, std::string user_info);
int bambu_network_change_user(void *agent, std::string user_info) {
  (void)agent;
  (void)user_info;
  return 0;
}

typedef bool (*func_is_user_login)(void *agent);
bool bambu_network_is_user_login(void *agent) {
  (void)agent;
  return false;
}

typedef int (*func_user_logout)(void *agent, bool request);
int bambu_network_user_logout(void *agent, bool request) {
  (void)agent;
  (void)request;
  return 0;
}

typedef std::string (*func_get_user_id)(void *agent);
std::string bambu_network_get_user_id(void *agent) {
  (void)agent;
  return "";
}

typedef std::string (*func_get_user_name)(void *agent);
std::string bambu_network_get_user_name(void *agent) {
  (void)agent;
  return "";
}

typedef std::string (*func_get_user_avatar)(void *agent);
std::string bambu_network_get_user_avatar(void *agent) {
  (void)agent;
  return "";
}

typedef std::string (*func_get_user_nickanme)(void *agent);
std::string bambu_network_get_user_nickanme(void *agent) {
  (void)agent;
  return "";
}

typedef std::string (*func_build_login_cmd)(void *agent);
std::string bambu_network_build_login_cmd(void *agent) {
  (void)agent;
  return "";
}

typedef std::string (*func_build_logout_cmd)(void *agent);
std::string bambu_network_build_logout_cmd(void *agent) {
  (void)agent;
  return "";
}

typedef std::string (*func_build_login_info)(void *agent);
std::string bambu_network_build_login_info(void *agent) {
  (void)agent;
  return "";
}

typedef int (*func_get_model_id_from_desgin_id)(void *agent, std::string& desgin_id, std::string& model_id);
int bambu_network_get_model_id_from_desgin_id(void *agent, std::string& desgin_id, std::string& model_id) {
  (void)agent;
  (void)desgin_id;
  (void)model_id;
  return 0;
}

typedef int (*func_ping_bind)(void *agent, std::string ping_code);
int bambu_network_ping_bind(void *agent, std::string ping_code) {
  (void)agent;
  (void)ping_code;
  return 0;
}

typedef int (*func_bind_detect)(void *agent, std::string dev_ip, std::string sec_link, detectResult& detect);
int bambu_network_bind_detect(void *agent, std::string dev_ip, std::string sec_link, detectResult& detect) {
  (void)agent;
  (void)dev_ip;
  (void)sec_link;
  (void)detect;
  return 0;
}

typedef int (*func_set_server_callback)(void *agent, OnServerErrFn fn);
int bambu_network_set_server_callback(void *agent, OnServerErrFn fn) {
  (void)agent;
  (void)fn;
  return 0;
}

typedef int (*func_bind)(void *agent, std::string dev_ip, std::string dev_id, std::string sec_link, std::string timezone, bool improved, OnUpdateStatusFn update_fn);
int bambu_network_bind(void *agent, std::string dev_ip, std::string dev_id, std::string sec_link, std::string timezone, bool improved, OnUpdateStatusFn update_fn) {
  (void)agent;
  (void)dev_ip;
  (void)dev_id;
  (void)sec_link;
  (void)timezone;
  (void)improved;
  (void)update_fn;
  return 0;
}

typedef int (*func_unbind)(void *agent, std::string dev_id);
int bambu_network_unbind(void *agent, std::string dev_id) {
  (void)agent;
  (void)dev_id;
  return 0;
}

typedef std::string (*func_get_bambulab_host)(void *agent);
std::string bambu_network_get_bambulab_host(void *agent) {
  (void)agent;
  return "";
}

typedef std::string (*func_get_user_selected_machine)(void *agent);
std::string bambu_network_get_user_selected_machine(void *agent) {
  (void)agent;
  return "";
}

typedef int (*func_set_user_selected_machine)(void *agent, std::string dev_id);
int bambu_network_set_user_selected_machine(void *agent, std::string dev_id) {
  (void)agent;
  (void)dev_id;
  return 0;
}

typedef int (*func_start_print)(void *agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
int bambu_network_start_print(void *agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn) {
  (void)agent;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  (void)wait_fn;
  return 0;
}

typedef int (*func_start_local_print_with_record)(void *agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
int bambu_network_start_local_print_with_record(void *agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn) {
  (void)agent;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  (void)wait_fn;
  return 0;
}

typedef int (*func_start_send_gcode_to_sdcard)(void *agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
int bambu_network_start_send_gcode_to_sdcard(void *agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn) {
  (void)agent;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  (void)wait_fn;
  return 0;
}

typedef int (*func_start_local_print)(void *agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn);
int bambu_network_start_local_print(void *agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn) {
  (void)agent;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  return 0;
}

typedef int (*func_start_sdcard_print)(void *agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn);
int bambu_network_start_sdcard_print(void *agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn) {
  (void)agent;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  return 0;
}

typedef int (*func_get_user_presets)(void *agent, std::map<std::string, std::map<std::string, std::string>>* user_presets);
int bambu_network_get_user_presets(void *agent, std::map<std::string, std::map<std::string, std::string>>* user_presets) {
  (void)agent;
  (void)user_presets;
  return 0;
}

typedef std::string (*func_request_setting_id)(void *agent, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code);
std::string bambu_network_request_setting_id(void *agent, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code) {
  (void)agent;
  (void)name;
  (void)values_map;
  (void)http_code;
  return "";
}

typedef int (*func_put_setting)(void *agent, std::string setting_id, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code);
int bambu_network_put_setting(void *agent, std::string setting_id, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code) {
  (void)agent;
  (void)setting_id;
  (void)name;
  (void)values_map;
  (void)http_code;
  return 0;
}

typedef int (*func_get_setting_list)(void *agent, std::string bundle_version, ProgressFn pro_fn, WasCancelledFn cancel_fn);
int bambu_network_get_setting_list(void *agent, std::string bundle_version, ProgressFn pro_fn, WasCancelledFn cancel_fn) {
  (void)agent;
  (void)bundle_version;
  (void)pro_fn;
  (void)cancel_fn;
  return 0;
}

typedef int (*func_get_setting_list2)(void *agent, std::string bundle_version, CheckFn chk_fn, ProgressFn pro_fn, WasCancelledFn cancel_fn);
int bambu_network_get_setting_list2(void *agent, std::string bundle_version, CheckFn chk_fn, ProgressFn pro_fn, WasCancelledFn cancel_fn) {
  (void)agent;
  (void)bundle_version;
  (void)chk_fn;
  (void)pro_fn;
  (void)cancel_fn;
  return 0;
}

typedef int (*func_delete_setting)(void *agent, std::string setting_id);
int bambu_network_delete_setting(void *agent, std::string setting_id) {
  (void)agent;
  (void)setting_id;
  return 0;
}

typedef std::string (*func_get_studio_info_url)(void *agent);
std::string bambu_network_get_studio_info_url(void *agent) {
  (void)agent;
  return "";
}

typedef int (*func_set_extra_http_header)(void *agent, std::map<std::string, std::string> extra_headers);
int bambu_network_set_extra_http_header(void *agent, std::map<std::string, std::string> extra_headers) {
  (void)agent;
  (void)extra_headers;
  return 0;
}

typedef int (*func_get_my_message)(void *agent, int type, int after, int limit, unsigned int* http_code, std::string* http_body);
int bambu_network_get_my_message(void *agent, int type, int after, int limit, unsigned int* http_code, std::string* http_body) {
  (void)agent;
  (void)type;
  (void)after;
  (void)limit;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_check_user_task_report)(void *agent, int* task_id, bool* printable);
int bambu_network_check_user_task_report(void *agent, int* task_id, bool* printable) {
  (void)agent;
  (void)task_id;
  (void)printable;
  return 0;
}

typedef int (*func_get_user_print_info)(void *agent, unsigned int* http_code, std::string* http_body);
int bambu_network_get_user_print_info(void *agent, unsigned int* http_code, std::string* http_body) {
  (void)agent;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_get_user_tasks)(void *agent, TaskQueryParams params, std::string* http_body);
int bambu_network_get_user_tasks(void *agent, TaskQueryParams params, std::string* http_body) {
  (void)agent;
  (void)params;
  (void)http_body;
  return 0;
}

typedef int (*func_get_printer_firmware)(void *agent, std::string dev_id, unsigned* http_code, std::string* http_body);
int bambu_network_get_printer_firmware(void *agent, std::string dev_id, unsigned* http_code, std::string* http_body) {
  (void)agent;
  (void)dev_id;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_get_task_plate_index)(void *agent, std::string task_id, int* plate_index);
int bambu_network_get_task_plate_index(void *agent, std::string task_id, int* plate_index) {
  (void)agent;
  (void)task_id;
  (void)plate_index;
  return 0;
}

typedef int (*func_get_user_info)(void *agent, int* identifier);
int bambu_network_get_user_info(void *agent, int* identifier) {
  (void)agent;
  (void)identifier;
  return 0;
}

typedef int (*func_request_bind_ticket)(void *agent, std::string* ticket);
int bambu_network_request_bind_ticket(void *agent, std::string* ticket) {
  (void)agent;
  (void)ticket;
  return 0;
}

typedef int (*func_get_subtask_info)(void *agent, std::string subtask_id, std::string* task_json, unsigned int* http_code, std::string *http_body);
int bambu_network_get_subtask_info(void *agent, std::string subtask_id, std::string* task_json, unsigned int* http_code, std::string *http_body) {
  (void)agent;
  (void)subtask_id;
  (void)task_json;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_get_slice_info)(void *agent, std::string project_id, std::string profile_id, int plate_index, std::string* slice_json);
int bambu_network_get_slice_info(void *agent, std::string project_id, std::string profile_id, int plate_index, std::string* slice_json) {
  (void)agent;
  (void)project_id;
  (void)profile_id;
  (void)plate_index;
  (void)slice_json;
  return 0;
}

typedef int (*func_query_bind_status)(void *agent, std::vector<std::string> query_list, unsigned int* http_code, std::string* http_body);
int bambu_network_query_bind_status(void *agent, std::vector<std::string> query_list, unsigned int* http_code, std::string* http_body) {
  (void)agent;
  (void)query_list;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_modify_printer_name)(void *agent, std::string dev_id, std::string dev_name);
int bambu_network_modify_printer_name(void *agent, std::string dev_id, std::string dev_name) {
  (void)agent;
  (void)dev_id;
  (void)dev_name;
  return 0;
}

typedef int (*func_get_camera_url)(void *agent, std::string dev_id, std::function<void(std::string)> callback);
int bambu_network_get_camera_url(void *agent, std::string dev_id, std::function<void(std::string)> callback) {
  (void)agent;
  (void)dev_id;
  (void)callback;
  return 0;
}

typedef int (*func_get_design_staffpick)(void *agent, int offset, int limit, std::function<void(std::string)> callback);
int bambu_network_get_design_staffpick(void *agent, int offset, int limit, std::function<void(std::string)> callback) {
  (void)agent;
  (void)offset;
  (void)limit;
  (void)callback;
  return 0;
}

typedef int (*func_start_publish)(void *agent, PublishParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, std::string* out);
int bambu_network_start_publish(void *agent, PublishParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, std::string* out) {
  (void)agent;
  (void)params;
  (void)update_fn;
  (void)cancel_fn;
  (void)out;
  return 0;
}

typedef int (*func_get_profile_3mf)(void *agent, BBLProfile* profile);
int bambu_network_get_profile_3mf(void *agent, BBLProfile* profile) {
  (void)agent;
  (void)profile;
  return 0;
}

typedef int (*func_get_model_publish_url)(void *agent, std::string* url);
int bambu_network_get_model_publish_url(void *agent, std::string* url) {
  (void)agent;
  (void)url;
  return 0;
}

typedef int (*func_get_subtask)(void *agent, BBLModelTask* task, OnGetSubTaskFn getsub_fn);
int bambu_network_get_subtask(void *agent, BBLModelTask* task, OnGetSubTaskFn getsub_fn) {
  (void)agent;
  (void)task;
  (void)getsub_fn;
  return 0;
}

typedef int (*func_get_model_mall_home_url)(void *agent, std::string* url);
int bambu_network_get_model_mall_home_url(void *agent, std::string* url) {
  (void)agent;
  (void)url;
  return 0;
}

typedef int (*func_get_model_mall_detail_url)(void *agent, std::string* url, std::string id);
int bambu_network_get_model_mall_detail_url(void *agent, std::string* url, std::string id) {
  (void)agent;
  (void)url;
  (void)id;
  return 0;
}

typedef int (*func_get_my_profile)(void *agent, std::string token, unsigned int *http_code, std::string *http_body);
int bambu_network_get_my_profile(void *agent, std::string token, unsigned int *http_code, std::string *http_body) {
  (void)agent;
  (void)token;
  (void)http_code;
  (void)http_body;
  return 0;
}

typedef int (*func_track_enable)(void *agent, bool enable);
int bambu_network_track_enable(void *agent, bool enable) {
  (void)agent;
  (void)enable;
  return 0;
}

typedef int (*func_track_remove_files)(void *agent);
int bambu_network_track_remove_files(void *agent) {
  (void)agent;
  return 0;
}

typedef int (*func_track_event)(void *agent, std::string evt_key, std::string content);
int bambu_network_track_event(void *agent, std::string evt_key, std::string content) {
  (void)agent;
  (void)evt_key;
  (void)content;
  return 0;
}

typedef int (*func_track_header)(void *agent, std::string header);
int bambu_network_track_header(void *agent, std::string header) {
  (void)agent;
  (void)header;
  return 0;
}

typedef int (*func_track_update_property)(void *agent, std::string name, std::string value, std::string type);
int bambu_network_track_update_property(void *agent, std::string name, std::string value, std::string type) {
  (void)agent;
  (void)name;
  (void)value;
  (void)type;
  return 0;
}

typedef int (*func_track_get_property)(void *agent, std::string name, std::string& value, std::string type);
int bambu_network_track_get_property(void *agent, std::string name, std::string& value, std::string type) {
  (void)agent;
  (void)name;
  (void)value;
  (void)type;
  return 0;
}

typedef int (*func_put_model_mall_rating_url)(void *agent, int rating_id, int score, std::string content, std::vector<std::string> images, unsigned int &http_code, std::string &http_error);
int bambu_network_put_model_mall_rating(void *agent, int rating_id, int score, std::string content, std::vector<std::string> images, unsigned int &http_code, std::string &http_error) {
  (void)agent;
  (void)rating_id;
  (void)score;
  (void)content;
  (void)images;
  (void)http_code;
  (void)http_error;
  return 0;
}

typedef int (*func_get_oss_config)(void *agent, std::string &config, std::string country_code, unsigned int &http_code, std::string &http_error);
int bambu_network_get_oss_config(void *agent, std::string &config, std::string country_code, unsigned int &http_code, std::string &http_error) {
  (void)agent;
  (void)config;
  (void)country_code;
  (void)http_code;
  (void)http_error;
  return 0;
}

typedef int (*func_put_rating_picture_oss)(void *agent, std::string &config, std::string &pic_oss_path, std::string model_id, int profile_id, unsigned int &http_code, std::string &http_error);
int bambu_network_put_rating_picture_oss(void *agent, std::string &config, std::string &pic_oss_path, std::string model_id, int profile_id, unsigned int &http_code, std::string &http_error) {
  (void)agent;
  (void)config;
  (void)pic_oss_path;
  (void)model_id;
  (void)profile_id;
  (void)http_code;
  (void)http_error;
  return 0;
}

typedef int (*func_get_model_mall_rating_result)(void *agent, int job_id, std::string &rating_result, unsigned int &http_code, std::string &http_error);
int bambu_network_get_model_mall_rating(void *agent, int job_id, std::string &rating_result, unsigned int &http_code, std::string &http_error) {
  (void)agent;
  (void)job_id;
  (void)rating_result;
  (void)http_code;
  (void)http_error;
  return 0;
}

typedef int (*func_get_mw_user_preference)(void *agent, std::function<void(std::string)> callback);
int bambu_network_get_mw_user_preference(void *agent, std::function<void(std::string)> callback) {
  (void)agent;
  (void)callback;
  return 0;
}

typedef int (*func_get_mw_user_4ulist)(void *agent, int seed, int limit, std::function<void(std::string)> callback);
int bambu_network_get_mw_user_4ulist(void *agent, int seed, int limit, std::function<void(std::string)> callback) {
  (void)agent;
  (void)seed;
  (void)limit;
  (void)callback;
  return 0;
}

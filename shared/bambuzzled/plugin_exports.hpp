#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>

#include <bambuzzled/api/bambu_networking.hpp>
#include <bambuzzled/api/project_task.hpp>

#if defined(_WIN32) || defined(__CYGWIN__)
#define BBZL_EXPORT extern "C" __declspec(dllexport)
#else
#define BBZL_EXPORT extern "C" __attribute__((visibility("default")))
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4190)
#endif

BBZL_EXPORT bool bambu_network_check_debug_consistent(bool is_debug);
BBZL_EXPORT std::string bambu_network_get_version();
BBZL_EXPORT void* bambu_network_create_agent(std::string log_dir);
BBZL_EXPORT int bambu_network_destroy_agent(void* agent);
BBZL_EXPORT int bambu_network_init_log(void* agent);
BBZL_EXPORT int bambu_network_set_config_dir(void* agent, std::string config_dir);
BBZL_EXPORT int bambu_network_set_cert_file(void* agent, std::string folder, std::string filename);
BBZL_EXPORT int bambu_network_set_country_code(void* agent, std::string country_code);
BBZL_EXPORT int bambu_network_start(void* agent);
BBZL_EXPORT int bambu_network_set_on_ssdp_msg_fn(void* agent, OnMsgArrivedFn fn);
BBZL_EXPORT int bambu_network_set_on_user_login_fn(void* agent, OnUserLoginFn fn);
BBZL_EXPORT int bambu_network_set_on_printer_connected_fn(void* agent, OnPrinterConnectedFn fn);
BBZL_EXPORT int bambu_network_set_on_server_connected_fn(void* agent, OnServerConnectedFn fn);
BBZL_EXPORT int bambu_network_set_on_http_error_fn(void* agent, OnHttpErrorFn fn);
BBZL_EXPORT int bambu_network_set_get_country_code_fn(void* agent, GetCountryCodeFn fn);
BBZL_EXPORT int bambu_network_set_on_subscribe_failure_fn(void* agent, GetSubscribeFailureFn fn);
BBZL_EXPORT int bambu_network_set_on_message_fn(void* agent, OnMessageFn fn);
BBZL_EXPORT int bambu_network_set_on_user_message_fn(void* agent, OnMessageFn fn);
BBZL_EXPORT int bambu_network_set_on_local_connect_fn(void* agent, OnLocalConnectedFn fn);
BBZL_EXPORT int bambu_network_set_on_local_message_fn(void* agent, OnMessageFn fn);
BBZL_EXPORT int bambu_network_set_queue_on_main_fn(void* agent, QueueOnMainFn fn);
BBZL_EXPORT int bambu_network_connect_server(void* agent);
BBZL_EXPORT bool bambu_network_is_server_connected(void* agent);
BBZL_EXPORT int bambu_network_refresh_connection(void* agent);
BBZL_EXPORT int bambu_network_start_subscribe(void* agent, std::string module);
BBZL_EXPORT int bambu_network_stop_subscribe(void* agent, std::string module);
BBZL_EXPORT int bambu_network_add_subscribe(void* agent, std::vector<std::string> dev_list);
BBZL_EXPORT int bambu_network_del_subscribe(void* agent, std::vector<std::string> dev_list);
BBZL_EXPORT void bambu_network_enable_multi_machine(void* agent, bool enable);
BBZL_EXPORT int bambu_network_start_device_subscribe(void* agent);
BBZL_EXPORT int bambu_network_stop_device_subscribe(void* agent);
BBZL_EXPORT int bambu_network_send_message(void* agent, std::string dev_id, std::string json_str, int qos);
BBZL_EXPORT int bambu_network_connect_printer(void* agent, std::string dev_id, std::string dev_ip, std::string username, std::string password, bool use_ssl);
BBZL_EXPORT int bambu_network_disconnect_printer(void* agent);
BBZL_EXPORT int bambu_network_send_message_to_printer(void* agent, std::string dev_id, std::string json_str, int qos);
BBZL_EXPORT bool bambu_network_start_discovery(void* agent, bool start, bool sending);
BBZL_EXPORT int bambu_network_change_user(void* agent, std::string user_info);
BBZL_EXPORT bool bambu_network_is_user_login(void* agent);
BBZL_EXPORT int bambu_network_user_logout(void* agent, bool request);
BBZL_EXPORT std::string bambu_network_get_user_id(void* agent);
BBZL_EXPORT std::string bambu_network_get_user_name(void* agent);
BBZL_EXPORT std::string bambu_network_get_user_avatar(void* agent);
BBZL_EXPORT std::string bambu_network_get_user_nickanme(void* agent);
BBZL_EXPORT std::string bambu_network_build_login_cmd(void* agent);
BBZL_EXPORT std::string bambu_network_build_logout_cmd(void* agent);
BBZL_EXPORT std::string bambu_network_build_login_info(void* agent);
BBZL_EXPORT int bambu_network_get_model_id_from_desgin_id(void* agent, std::string& desgin_id, std::string& model_id);
BBZL_EXPORT int bambu_network_ping_bind(void* agent, std::string ping_code);
BBZL_EXPORT int bambu_network_bind_detect(void* agent, std::string dev_ip, std::string sec_link, detectResult& detect);
BBZL_EXPORT int bambu_network_set_server_callback(void* agent, OnServerErrFn fn);
BBZL_EXPORT int bambu_network_bind(void* agent, std::string dev_ip, std::string dev_id, std::string sec_link, std::string timezone, bool improved, OnUpdateStatusFn update_fn);
BBZL_EXPORT int bambu_network_unbind(void* agent, std::string dev_id);
BBZL_EXPORT std::string bambu_network_get_bambulab_host(void* agent);
BBZL_EXPORT std::string bambu_network_get_user_selected_machine(void* agent);
BBZL_EXPORT int bambu_network_set_user_selected_machine(void* agent, std::string dev_id);
BBZL_EXPORT int bambu_network_start_print(void* agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
BBZL_EXPORT int bambu_network_start_local_print_with_record(void* agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
BBZL_EXPORT int bambu_network_start_send_gcode_to_sdcard(void* agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
BBZL_EXPORT int bambu_network_start_local_print(void* agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn);
BBZL_EXPORT int bambu_network_start_sdcard_print(void* agent, PrintParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn);
BBZL_EXPORT int bambu_network_get_user_presets(void* agent, std::map<std::string, std::map<std::string, std::string>>* user_presets);
BBZL_EXPORT std::string bambu_network_request_setting_id(void* agent, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code);
BBZL_EXPORT int bambu_network_put_setting(void* agent, std::string setting_id, std::string name, std::map<std::string, std::string>* values_map, unsigned int* http_code);
BBZL_EXPORT int bambu_network_get_setting_list(void* agent, std::string bundle_version, ProgressFn pro_fn, WasCancelledFn cancel_fn);
BBZL_EXPORT int bambu_network_get_setting_list2(void* agent, std::string bundle_version, CheckFn chk_fn, ProgressFn pro_fn, WasCancelledFn cancel_fn);
BBZL_EXPORT int bambu_network_delete_setting(void* agent, std::string setting_id);
BBZL_EXPORT std::string bambu_network_get_studio_info_url(void* agent);
BBZL_EXPORT int bambu_network_set_extra_http_header(void* agent, std::map<std::string, std::string> extra_headers);
BBZL_EXPORT int bambu_network_get_my_message(void* agent, int type, int after, int limit, unsigned int* http_code, std::string* http_body);
BBZL_EXPORT int bambu_network_check_user_task_report(void* agent, int* task_id, bool* printable);
BBZL_EXPORT int bambu_network_get_user_print_info(void* agent, unsigned int* http_code, std::string* http_body);
BBZL_EXPORT int bambu_network_get_user_tasks(void* agent, TaskQueryParams params, std::string* http_body);
BBZL_EXPORT int bambu_network_get_printer_firmware(void* agent, std::string dev_id, unsigned* http_code, std::string* http_body);
BBZL_EXPORT int bambu_network_get_task_plate_index(void* agent, std::string task_id, int* plate_index);
BBZL_EXPORT int bambu_network_get_user_info(void* agent, int* identifier);
BBZL_EXPORT int bambu_network_request_bind_ticket(void* agent, std::string* ticket);
BBZL_EXPORT int bambu_network_get_subtask_info(void* agent, std::string subtask_id, std::string* task_json, unsigned int* http_code, std::string* http_body);
BBZL_EXPORT int bambu_network_get_slice_info(void* agent, std::string project_id, std::string profile_id, int plate_index, std::string* slice_json);
BBZL_EXPORT int bambu_network_query_bind_status(void* agent, std::vector<std::string> query_list, unsigned int* http_code, std::string* http_body);
BBZL_EXPORT int bambu_network_modify_printer_name(void* agent, std::string dev_id, std::string dev_name);
BBZL_EXPORT int bambu_network_get_camera_url(void* agent, std::string dev_id, std::function<void(std::string)> callback);
BBZL_EXPORT int bambu_network_get_design_staffpick(void* agent, int offset, int limit, std::function<void(std::string)> callback);
BBZL_EXPORT int bambu_network_start_publish(void* agent, PublishParams params, OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, std::string* out);
BBZL_EXPORT int bambu_network_get_profile_3mf(void* agent, BBLProfile* profile);
BBZL_EXPORT int bambu_network_get_model_publish_url(void* agent, std::string* url);
BBZL_EXPORT int bambu_network_get_subtask(void* agent, BBLModelTask* task, OnGetSubTaskFn getsub_fn);
BBZL_EXPORT int bambu_network_get_model_mall_home_url(void* agent, std::string* url);
BBZL_EXPORT int bambu_network_get_model_mall_detail_url(void* agent, std::string* url, std::string id);
BBZL_EXPORT int bambu_network_get_my_profile(void* agent, std::string token, unsigned int* http_code, std::string* http_body);
BBZL_EXPORT int bambu_network_track_enable(void* agent, bool enable);
BBZL_EXPORT int bambu_network_track_remove_files(void* agent);
BBZL_EXPORT int bambu_network_track_event(void* agent, std::string evt_key, std::string content);
BBZL_EXPORT int bambu_network_track_header(void* agent, std::string header);
BBZL_EXPORT int bambu_network_track_update_property(void* agent, std::string name, std::string value, std::string type);
BBZL_EXPORT int bambu_network_track_get_property(void* agent, std::string name, std::string& value, std::string type);
BBZL_EXPORT int bambu_network_put_model_mall_rating(void* agent, int rating_id, int score, std::string content, std::vector<std::string> images, unsigned int& http_code, std::string& http_error);
BBZL_EXPORT int bambu_network_get_oss_config(void* agent, std::string& config, std::string country_code, unsigned int& http_code, std::string& http_error);
BBZL_EXPORT int bambu_network_put_rating_picture_oss(void* agent, std::string& config, std::string& pic_oss_path, std::string model_id, int profile_id, unsigned int& http_code, std::string& http_error);
BBZL_EXPORT int bambu_network_get_model_mall_rating(void* agent, int job_id, std::string& rating_result, unsigned int& http_code, std::string& http_error);
BBZL_EXPORT int bambu_network_get_mw_user_preference(void* agent, std::function<void(std::string)> callback);
BBZL_EXPORT int bambu_network_get_mw_user_4ulist(void* agent, int seed, int limit, std::function<void(std::string)> callback);

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

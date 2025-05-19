#pragma once

#include <mosquitto.h>

namespace mosq_c {

using ::mosquitto;
using ::mosquitto_message;
using ::mosquitto_new;
using ::mosquitto_destroy;
using ::mosquitto_loop;
using ::mosquitto_strerror;
using ::mosquitto_connack_string;
using ::mosquitto_reason_string;
using ::mosquitto_threaded_set;
using ::mosquitto_int_option;
using ::mosquitto_void_option;
using ::mosquitto_username_pw_set;
using ::mosquitto_connect_callback_set;
using ::mosquitto_disconnect_callback_set;
using ::mosquitto_subscribe_callback_set;
using ::mosquitto_message_callback_set;
using ::mosquitto_connect;
using ::mosquitto_disconnect;
using ::mosquitto_subscribe;
using ::mosquitto_publish;
using enum ::mosq_err_t;
using enum ::mosq_opt_t;

#undef MQTT_PROTOCOL_V31
#undef MQTT_PROTOCOL_V311
#undef MQTT_PROTOCOL_V5

enum mosq_protocol_t {
  MQTT_PROTOCOL_V31 = 3,
  MQTT_PROTOCOL_V311 = 4,
  MQTT_PROTOCOL_V5 = 5
};

}

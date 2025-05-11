#include <string>
#include <stdexcept>
#include <format>

#include <bambuzzled/ssl.hpp>

#include "mosquittopp.hpp"

namespace bbzl::mqtt {

using bbzl::ssl::openssl_ctx_handle;

openssl_ctx_handle create_openssl_context() {
  SSL_METHOD const* method = TLS_client_method();
  auto ctx = openssl_ctx_handle(SSL_CTX_new(method));
  SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

  if (ctx == nullptr) {
    throw std::runtime_error(std::format(
      "failed to create OpenSSL context: {}",
      bbzl::ssl::get_err_msg()
    ));
  }

  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, nullptr);

  X509_STORE* store = SSL_CTX_get_cert_store(ctx);
  X509_STORE_set_flags(store, X509_V_FLAG_NO_CHECK_TIME);

  X509_LOOKUP* lookup = X509_STORE_add_lookup(
    store,
    X509_LOOKUP_hash_dir()
  );
  X509_LOOKUP_add_dir(
    lookup,
    nullptr,
    X509_FILETYPE_DEFAULT
  );
  X509_LOOKUP_add_dir(
    lookup,
    reinterpret_cast<char const*>(
      bbzl::ssl::get_certs_dir().u8string().c_str()
    ),
    X509_FILETYPE_PEM
  );

  return ctx;
}

void on_connect(
  mosquitto*,
  void* obj,
  int reason_code
) noexcept {
  auto& mosqpp = *static_cast<mosquittopp*>(obj);
  if (mosqpp.on_connect_cb != nullptr) {
    mosqpp.on_connect_cb(mosqpp, reason_code);
  }
}

void on_disconnect(
  mosquitto*,
  void* obj,
  int reason_code
) noexcept {
  auto& mosqpp = *static_cast<mosquittopp*>(obj);
  if (mosqpp.on_disconnect_cb != nullptr) {
    mosqpp.on_disconnect_cb(mosqpp, reason_code);
  }
}

void on_suback(
  mosquitto*,
  void* obj,
  int mid,
  int qos_count,
  int const* granted_qos
) noexcept {
  auto& mosqpp = *static_cast<mosquittopp*>(obj);
  if (mosqpp.on_suback_cb != nullptr) {
    auto qos_arr = std::span(
      granted_qos,
      qos_count
    );
    mosqpp.on_suback_cb(mosqpp, mid, qos_arr);
  }
}

void on_message(
  mosquitto*,
  void* obj,
  mosquitto_message const* msg
) noexcept {
  auto& mosqpp = *static_cast<mosquittopp*>(obj);
  if (mosqpp.on_message_cb != nullptr) {
    mosqpp.on_message_cb(mosqpp, msg);
  }
}

mosquittopp::mosquittopp() {
  int rc;

  this->mosq = mosquitto_handle(mosquitto_new(nullptr, true, this));
  if (this->mosq == nullptr) {
    throw std::runtime_error("failed to create Mosquitto instance");
  }

  rc = mosquitto_threaded_set(this->mosq, true);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to enable Mosquitto threaded mode: {}",
      mosquitto_strerror(rc)
    ));
  }

  this->openssl_ctx = create_openssl_context();
  rc = mosquitto_int_option(this->mosq, MOSQ_OPT_SSL_CTX_WITH_DEFAULTS, 0);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to disable default Mosquitto OpenSSL context: {}",
      mosquitto_strerror(rc)
    ));
  }

  rc = mosquitto_void_option(this->mosq, MOSQ_OPT_SSL_CTX, this->openssl_ctx);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to set Mosquitto OpenSSL context: {}",
      mosquitto_strerror(rc)
    ));
  }

  rc = mosquitto_int_option(this->mosq, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V31);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to set Mosquitto protocol version: {}",
      mosquitto_strerror(rc)
    ));
  }

  mosquitto_connect_callback_set(this->mosq, on_connect);
  mosquitto_disconnect_callback_set(this->mosq, on_disconnect);
  mosquitto_subscribe_callback_set(this->mosq, on_suback);
  mosquitto_message_callback_set(this->mosq, on_message);
}

mosquittopp::~mosquittopp() {
  this->disconnect();
}

mosquitto* mosquittopp::get() {
  return this->mosq.get();
}

void mosquittopp::thread_main(std::stop_token st) {
  int rc;
  int err;

  // sleep for a day if keepalive is disabled
  int timeout = 1000 * 86400;
  if (this->keepalive > 0) {
    timeout = this->keepalive * 1000;
  }

  #if !defined(_WIN32)
  timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 10000000;
  #endif

  while (true) {
    rc = mosquitto_loop(mosq, timeout, 1);
    if (rc != MOSQ_ERR_NO_CONN) {
      break;
    }

    #if defined(_WIN32)
    Sleep(10);
    #else
    nanosleep(&ts, NULL);
    #endif
  }

  while (true) {
    do {
      rc = mosquitto_loop(this->mosq, timeout, 1);
      err = errno;
    } while (!st.stop_requested() && rc == MOSQ_ERR_SUCCESS);

    bool fatal_error = false;
    switch (rc) {
      case MOSQ_ERR_NOMEM:
      case MOSQ_ERR_PROTOCOL:
      case MOSQ_ERR_INVAL:
      case MOSQ_ERR_NOT_FOUND:
      case MOSQ_ERR_TLS:
      case MOSQ_ERR_PAYLOAD_SIZE:
      case MOSQ_ERR_NOT_SUPPORTED:
      case MOSQ_ERR_AUTH:
      case MOSQ_ERR_ACL_DENIED:
      case MOSQ_ERR_UNKNOWN:
      case MOSQ_ERR_EAI:
      case MOSQ_ERR_PROXY: {
        fatal_error = true;
        break;
      }
      case MOSQ_ERR_ERRNO: {
        if (err == EPROTO) {
          fatal_error = true;
        }
        break;
      }
    }

    if (fatal_error) {
      break;
    }

    if (st.stop_requested()) {
      break;
    }

    // TODO: auto reconnect
    break; // for now just break on disconnect...
  }
}

void mosquittopp::set_on_connect_cb(on_connect_cb_fn&& fn) {
  this->on_connect_cb = std::move(fn);
}

void mosquittopp::set_on_disconnect_cb(on_disconnect_cb_fn&& fn) {
  this->on_disconnect_cb = std::move(fn);
}

void mosquittopp::set_on_suback_cb(on_suback_cb_fn&& fn) {
  this->on_suback_cb = std::move(fn);
}

void mosquittopp::set_on_message_cb(on_message_cb_fn&& fn) {
  this->on_message_cb = std::move(fn);
}

void mosquittopp::connect(
  std::string& addr,
  uint16_t port,
  std::string& username,
  std::string& password,
  int keepalive
) {
  int rc;

  if (this->thread.joinable()) {
    throw std::runtime_error("already connected");
  }

  this->thread = std::jthread([this](std::stop_token st) {
    this->thread_main(st);
  });

  rc = mosquitto_username_pw_set(this->mosq, username.c_str(), password.c_str());
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error("failed to set MQTT credentials");
  }

  rc = mosquitto_connect(this->mosq, addr.c_str(), port, keepalive);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to connect to MQTT broker: {}",
      mosquitto_strerror(rc)
    ));
  }

  this->keepalive = keepalive;
}

void mosquittopp::disconnect() {
  if (!this->thread.joinable()) {
    return;
  }

  int rc = mosquitto_disconnect(this->mosq);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to request disconnect: ",
      mosquitto_strerror(rc)
    ));
  }

  (void)this->thread.get_stop_source().request_stop();
}

message_id mosquittopp::subscribe(std::string const& topic, int qos) {
  message_id mid;
  int rc = mosquitto_subscribe(this->mosq, &mid, topic.c_str(), qos);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed subscribing to topic: {}",
      mosquitto_strerror(rc)
    ));
  }

  return mid;
}

message_id mosquittopp::publish(
  std::string const& topic,
  const std::span<const std::byte>& payload,
  int qos,
  bool retain
) {
  auto payload_ptr = payload.data();
  auto payload_size = payload.size_bytes();
  if (payload_size > INT_MAX) {
    throw std::runtime_error("payload is too large");
  }

  message_id mid;
  int rc = mosquitto_publish(
    this->mosq,
    &mid,
    topic.c_str(),
    static_cast<int>(payload_size),
    payload_ptr,
    qos,
    retain
  );
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to publish message: {}",
      mosquitto_strerror(rc)
    ));
  }

  return mid;
}

message_id mosquittopp::publish(
  std::string const& topic,
  std::string const& payload,
  int qos,
  bool retain
) {
  auto span = std::span(
    reinterpret_cast<std::byte const*>(payload.data()),
    payload.length()
  );
  return this->publish(topic, span, qos, retain);
}

}

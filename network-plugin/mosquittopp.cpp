#include <cstddef>
#include <span>
#include <ctime>
#include <atomic>
#include <thread>
#include <format>
#include <string>
#include <cerrno>
#include <climits>
#include <cstdint>
#include <utility>
#include <exception>
#include <stdexcept>
#include <stop_token>
#include <bambuzzled/log.hpp>
#include <bambuzzled/ssl.hpp>
#include <openssl/crypto.h>
#include <openssl/prov_ssl.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include "mosquitto.hpp"
#include "mosquittopp.hpp"

namespace bbzl::mqtt {

using bbzl::utils::net::openssl_ctx_handle;

static openssl_ctx_handle create_openssl_context() {
  auto ctx = openssl_ctx_handle(
    SSL_CTX_new(TLS_client_method())
  );
  if (ctx == nullptr) {
    throw std::runtime_error(std::format(
      "failed to create ssl context: {}",
      bbzl::utils::net::get_ssl_error_messages()
    ));
  }

  SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
  SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
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
      bbzl::utils::net::get_certs_dir().u8string().c_str()
    ),
    X509_FILETYPE_PEM
  );

  return ctx;
}

void on_connect(
  mosq_c::mosquitto*,
  void* obj,
  int reason_code
) noexcept {
  auto& mosqpp = *static_cast<mosquittopp*>(obj);
  mosqpp.connected_ = true;
  if (mosqpp.on_connect_cb_ != nullptr) {
    mosqpp.on_connect_cb_(mosqpp, reason_code);
  }
}

void on_disconnect(
  mosq_c::mosquitto*,
  void* obj,
  int reason_code
) noexcept {
  auto& mosqpp = *static_cast<mosquittopp*>(obj);
  mosqpp.connected_ = false;
  if (mosqpp.on_disconnect_cb_ != nullptr) {
    mosqpp.on_disconnect_cb_(mosqpp, reason_code);
  }
}

void on_suback(
  mosq_c::mosquitto*,
  void* obj,
  int mid,
  int qos_count,
  int const* granted_qos
) noexcept {
  auto& mosqpp = *static_cast<mosquittopp*>(obj);
  if (mosqpp.on_suback_cb_ != nullptr) {
    auto qos_arr = std::span(
      granted_qos,
      qos_count
    );
    mosqpp.on_suback_cb_(mosqpp, mid, qos_arr);
  }
}

void on_message(
  mosq_c::mosquitto*,
  void* obj,
  mosq_c::mosquitto_message const* msg
) noexcept {
  auto& mosqpp = *static_cast<mosquittopp*>(obj);
  if (mosqpp.on_message_cb_ != nullptr) {
    mosqpp.on_message_cb_(mosqpp, msg);
  }
}

mosquittopp::mosquittopp() {
  int r;

  auto mosq = mosquitto_handle(
    mosq_c::mosquitto_new(nullptr, true, this)
  );
  if (mosq == nullptr) {
    throw std::runtime_error("failed to create mosquitto instance");
  }

  r = mosq_c::mosquitto_threaded_set(mosq, true);
  if (r != mosq_c::MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to enable mosquitto threaded mode: {}",
      mosq_c::mosquitto_strerror(r)
    ));
  }

  auto ctx = create_openssl_context();
  r = mosq_c::mosquitto_int_option(
    mosq,
    mosq_c::MOSQ_OPT_SSL_CTX_WITH_DEFAULTS,
    0
  );
  if (r != mosq_c::MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to disable default mosquitto ssl context: {}",
      mosq_c::mosquitto_strerror(r)
    ));
  }

  r = mosq_c::mosquitto_void_option(
    mosq,
    mosq_c::MOSQ_OPT_SSL_CTX,
    ctx
  );
  if (r != mosq_c::MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to set mosquitto ssl context: {}",
      mosq_c::mosquitto_strerror(r)
    ));
  }

  r = mosq_c::mosquitto_int_option(
    mosq,
    mosq_c::MOSQ_OPT_PROTOCOL_VERSION,
    mosq_c::MQTT_PROTOCOL_V31
  );
  if (r != mosq_c::MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to set mosquitto protocol version: {}",
      mosq_c::mosquitto_strerror(r)
    ));
  }

  mosq_c::mosquitto_connect_callback_set(mosq, on_connect);
  mosq_c::mosquitto_disconnect_callback_set(mosq, on_disconnect);
  mosq_c::mosquitto_subscribe_callback_set(mosq, on_suback);
  mosq_c::mosquitto_message_callback_set(mosq, on_message);

  this->mosq_ = std::move(mosq);
  this->ssl_ctx_ = std::move(ctx);
}

mosquittopp::~mosquittopp() {
  this->disconnect();
}

void mosquittopp::thread_main(
  std::stop_token&& st,
  std::string const&& addr,
  uint16_t port
) {
  int r;
  int err;

  // sleep for a day if keepalive is disabled
  int timeout = 1000 * 86400;
  if (this->keepalive_ > 0) {
    timeout = this->keepalive_ * 1000;
  }

  #if !defined(_WIN32)
  timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 10000000;
  #endif

  while (true) {
    r = mosq_c::mosquitto_loop(this->mosq_, timeout, 1);
    if (r != mosq_c::MOSQ_ERR_NO_CONN) {
      break;
    }

    #if defined(_WIN32)
    ::Sleep(10);
    #else
    ::nanosleep(&ts, NULL);
    #endif
  }

  while (true) {
    do {
      r = mosq_c::mosquitto_loop(this->mosq_, timeout, 1);
      err = errno;
    } while (!st.stop_requested() && r == mosq_c::MOSQ_ERR_SUCCESS);

    bool fatal_error = false;
    switch (r) {
      case mosq_c::MOSQ_ERR_NOMEM:
      case mosq_c::MOSQ_ERR_PROTOCOL:
      case mosq_c::MOSQ_ERR_INVAL:
      case mosq_c::MOSQ_ERR_NOT_FOUND:
      case mosq_c::MOSQ_ERR_TLS:
      case mosq_c::MOSQ_ERR_PAYLOAD_SIZE:
      case mosq_c::MOSQ_ERR_NOT_SUPPORTED:
      case mosq_c::MOSQ_ERR_AUTH:
      case mosq_c::MOSQ_ERR_ACL_DENIED:
      case mosq_c::MOSQ_ERR_UNKNOWN:
      case mosq_c::MOSQ_ERR_EAI:
      case mosq_c::MOSQ_ERR_PROXY: {
        fatal_error = true;
        break;
      }
      case mosq_c::MOSQ_ERR_ERRNO: {
        if (err == EPROTO) {
          fatal_error = true;
        }
        break;
      }
    }

    if (fatal_error) {
      bbzl::log::error_fmt(
        "({}:{}) mosquitto thread caught fatal error: {}",
        addr,
        port,
        mosq_c::mosquitto_strerror(r)
      );
      break;
    }

    if (st.stop_requested()) {
      bbzl::log::debug_fmt(
        "({}:{}) mosquitto thread caught stop request",
        addr,
        port
      );
      break;
    }

    // TODO: auto reconnect
    break; // for now just break on disconnect...
  }
}

void mosquittopp::set_on_connect_cb(on_connect_cb_fn&& fn) {
  this->on_connect_cb_ = std::move(fn);
}

void mosquittopp::set_on_disconnect_cb(on_disconnect_cb_fn&& fn) {
  this->on_disconnect_cb_ = std::move(fn);
}

void mosquittopp::set_on_suback_cb(on_suback_cb_fn&& fn) {
  this->on_suback_cb_ = std::move(fn);
}

void mosquittopp::set_on_message_cb(on_message_cb_fn&& fn) {
  this->on_message_cb_ = std::move(fn);
}

bool mosquittopp::active() const noexcept {
  return this->thread_active_;
}

bool mosquittopp::connected() const noexcept {
  return this->connected_;
}

void mosquittopp::connect(
  std::string const& addr,
  uint16_t port,
  std::string const& username,
  std::string const& password,
  int keepalive
) {
  int r;

  if (this->active()) {
    throw std::runtime_error("already connected or connecting");
  }

  this->active_addr_ = addr;
  this->active_port_ = port;
  this->thread_active_.store(true, std::memory_order_seq_cst);
  this->thread_ = std::jthread([
    this,
    addr(addr),
    port
  ](std::stop_token st) {
    try {
      bbzl::log::debug_fmt(
        "({}:{}) mosquitto thread started",
        addr,
        port
      );
      this->thread_main(std::move(st), std::string(addr), port);
    } catch (std::exception& ex) {
      bbzl::log::error_fmt(
        "({}:{}) caught exception in mosquitto thread: {}",
        addr,
        port,
        ex.what()
      );
    }
    this->thread_active_.store(false, std::memory_order_seq_cst);
    bbzl::log::debug_fmt(
      "({}:{}) mosquitto thread stopped",
      addr,
      port
    );
  });

  r = mosq_c::mosquitto_username_pw_set(this->mosq_, username.c_str(), password.c_str());
  if (r != mosq_c::MOSQ_ERR_SUCCESS) {
    throw std::runtime_error("failed to set MQTT credentials");
  }

  bbzl::log::trace_fmt(
    "({}:{}) connecting to MQTT broker",
    this->active_addr_,
    this->active_port_
  );

  r = mosq_c::mosquitto_connect(this->mosq_, addr.c_str(), port, 0);
  if (r != mosq_c::MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to connect to MQTT broker: {}",
      mosq_c::mosquitto_strerror(r)
    ));
  }

  this->keepalive_ = keepalive;
}

void mosquittopp::disconnect() {
  if (this->connected()) {
    bbzl::log::trace_fmt(
      "({}:{}) requesting disconnect",
      this->active_addr_,
      this->active_port_
    );
    int r = mosq_c::mosquitto_disconnect(this->mosq_);
    if (r != mosq_c::MOSQ_ERR_SUCCESS) {
      throw std::runtime_error(std::format(
        "failed to request disconnect: ",
        mosq_c::mosquitto_strerror(r)
      ));
    }
  }

  if (this->active()) {
    bbzl::log::trace_fmt(
      "({}:{}) requesting mosquitto thread stop",
      this->active_addr_,
      this->active_port_
    );
    (void)this->thread_.get_stop_source().request_stop();
  }
}

message_id mosquittopp::subscribe(std::string const& topic, int qos) {
  message_id mid;
  int r = mosq_c::mosquitto_subscribe(
    this->mosq_,
    &mid,
    topic.c_str(),
    qos
  );
  if (r != mosq_c::MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed subscribing to topic: {}",
      mosq_c::mosquitto_strerror(r)
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
    throw std::length_error("payload is too large");
  }

  message_id mid;
  int r = mosq_c::mosquitto_publish(
    this->mosq_,
    &mid,
    topic.c_str(),
    static_cast<int>(payload_size),
    payload_ptr,
    qos,
    retain
  );
  if (r != mosq_c::MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(std::format(
      "failed to publish message: {}",
      mosq_c::mosquitto_strerror(r)
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

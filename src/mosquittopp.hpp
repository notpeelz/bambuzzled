#pragma once

#include <memory>
#include <thread>
#include <string>
#include <span>
#include <functional>

#include <openssl/ssl.h>
#include <mosquitto.h>

namespace bbzl::mqtt {

struct openssl_ctx_deleter {
  void operator()(SSL_CTX* ctx) const {
    if (ctx != nullptr) {
      SSL_CTX_free(ctx);
    }
  }
};

struct mosquitto_deleter {
  void operator()(mosquitto* mosq) const {
    if (mosq != nullptr) {
      mosquitto_destroy(mosq);
    }
  }
};

using openssl_ctx_ptr = std::unique_ptr<SSL_CTX, openssl_ctx_deleter>;
using mosquitto_ptr = std::unique_ptr<mosquitto, mosquitto_deleter>;
using message_id = int;

class mosquittopp;

using on_connect_cb_fn = std::function<void(
  mosquittopp& mosq,
  int reason_code
)>;

using on_disconnect_cb_fn = std::function<void(
  mosquittopp& mosq,
  int reason_code
)>;

using on_suback_cb_fn = std::function<void(
  mosquittopp& mosq,
  message_id mid,
  const std::span<const int>& qos_arr
)>;

using on_message_cb_fn = std::function<void(
  mosquittopp& mosq,
  const mosquitto_message* msg
)>;

class mosquittopp {
private:
  openssl_ctx_ptr openssl_ctx;
  on_connect_cb_fn on_connect_cb;
  on_disconnect_cb_fn on_disconnect_cb;
  on_suback_cb_fn on_suback_cb;
  on_message_cb_fn on_message_cb;
  mosquitto_ptr mosq;
  int keepalive = 0;
  void thread_main(std::stop_token st);
  std::jthread thread;

  friend void on_connect(
    mosquitto*,
    void* obj,
    int reason_code
  ) noexcept;
  friend void on_disconnect(
    mosquitto*,
    void* obj,
    int reason_code
  ) noexcept;
  friend void on_message(
    mosquitto*,
    void* obj,
    const mosquitto_message* msg
  ) noexcept;
  friend void on_suback(
    mosquitto*,
    void* obj,
    int mid,
    int qos_count,
    const int* granted_qos
  ) noexcept;
public:
  mosquittopp();
  ~mosquittopp();
  mosquitto* get();
  void set_on_connect_cb(on_connect_cb_fn&& fn);
  void set_on_disconnect_cb(on_disconnect_cb_fn&& fn);
  void set_on_suback_cb(on_suback_cb_fn&& fn);
  void set_on_message_cb(on_message_cb_fn&& fn);
  void connect(
    std::string& addr,
    uint16_t port,
    std::string& username,
    std::string& password,
    int keepalive = 0
  );
  void disconnect();
  message_id subscribe(const std::string& topic, int qos);
  message_id publish(
    const std::string& topic,
    const std::span<const std::byte>& payload,
    int qos,
    bool retain = false
  );
  message_id publish(
    const std::string& topic,
    const std::string& payload,
    int qos,
    bool retain = false
  );
};

}

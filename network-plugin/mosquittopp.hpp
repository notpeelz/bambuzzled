#pragma once

#include <memory>
#include <thread>
#include <string>
#include <span>
#include <functional>

#include <openssl/ssl.h>
#include <mosquitto.h>
#include <bambuzzled/ssl.hpp>

namespace bbzl::mqtt {

struct mosquitto_handle_deleter {
  void operator()(mosquitto* mosq) const {
    if (mosq != nullptr) {
      mosquitto_destroy(mosq);
    }
  }
};

class mosquitto_handle
  : public std::unique_ptr<mosquitto, mosquitto_handle_deleter>
{
public:
  using base = std::unique_ptr<mosquitto, mosquitto_handle_deleter>;
  using base::base;
  operator mosquitto*() const { return this->get(); }
};

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
  mosquitto_message const* msg
)>;

class mosquittopp {
private:
  bbzl::ssl::openssl_ctx_handle openssl_ctx_;
  on_connect_cb_fn on_connect_cb_;
  on_disconnect_cb_fn on_disconnect_cb_;
  on_suback_cb_fn on_suback_cb_;
  on_message_cb_fn on_message_cb_;
  mosquitto_handle mosq_;
  int keepalive_ = 0;
  bool connected_ = false;
  void thread_main(std::stop_token st);
  std::jthread thread_;

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
    mosquitto_message const* msg
  ) noexcept;
  friend void on_suback(
    mosquitto*,
    void* obj,
    int mid,
    int qos_count,
    int const* granted_qos
  ) noexcept;
public:
  mosquittopp();
  ~mosquittopp();
  mosquitto* get();
  void set_on_connect_cb(on_connect_cb_fn&& fn);
  void set_on_disconnect_cb(on_disconnect_cb_fn&& fn);
  void set_on_suback_cb(on_suback_cb_fn&& fn);
  void set_on_message_cb(on_message_cb_fn&& fn);
  bool active();
  bool connected();
  void connect(
    std::string& addr,
    uint16_t port,
    std::string& username,
    std::string& password,
    int keepalive = 0
  );
  void disconnect();
  message_id subscribe(std::string const& topic, int qos);
  message_id publish(
    std::string const& topic,
    const std::span<const std::byte>& payload,
    int qos,
    bool retain = false
  );
  message_id publish(
    std::string const& topic,
    std::string const& payload,
    int qos,
    bool retain = false
  );
};

}

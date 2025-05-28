#pragma once

#include <span>
#include <format>
#include <memory>
#include <thread>
#include <string>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <functional>
#include <stop_token>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/optional_ref.hpp>
#include "mosquitto.hpp"

namespace bbzl::mqtt {

using mosq_c::mosquitto_strerror;
using mosq_c::mosquitto_connack_string;
using mosq_c::mosquitto_reason_string;

class mosquittopp_error
  : public std::runtime_error
{
private:
  std::string reason_;

public:
  mosquittopp_error(
    std::string const& msg,
    std::string const& reason
  )
    : std::runtime_error(std::format("{}: {}", msg, reason)),
      reason_(reason) {}

  mosquittopp_error(std::string const& msg)
    : std::runtime_error(msg) {}

  [[nodiscard]]
  bbzl::utils::optional_ref<std::string const> reason() {
    return std::make_optional(std::cref(this->reason_));
  }
};

class connect_failed
  : public mosquittopp_error
{
private:
  static constexpr char const msg[] = "failed to connect";

public:
  connect_failed(std::string const& reason)
    : mosquittopp_error(msg, reason) {}

  connect_failed()
    : mosquittopp_error(msg) {}
};

class disconnect_failed
  : public mosquittopp_error
{
private:
  static constexpr char const msg[] = "failed to disconnect";

public:
  disconnect_failed(std::string const& reason)
    : mosquittopp_error(msg, reason) {}

  disconnect_failed()
    : mosquittopp_error(msg) {}
};

class connect_in_progress
  : public mosquittopp_error
{
private:
  std::string reason_;
  static constexpr char const msg[] = "failed to connect";

public:
  connect_in_progress(std::string const& reason)
    : mosquittopp_error(msg, reason) {}

  connect_in_progress()
    : mosquittopp_error(msg) {}
};

class subscribe_failed
  : public mosquittopp_error
{
private:
  static constexpr char const msg[] = "failed subscribing to topic";

public:
  subscribe_failed(std::string const& reason)
    : mosquittopp_error(msg, reason) {}

  subscribe_failed()
    : mosquittopp_error(msg) {}
};

class publish_failed
  : public mosquittopp_error
{
private:
  static constexpr char const msg[] = "failed to publish message";

public:
  publish_failed(std::string const& reason)
    : mosquittopp_error(msg, reason) {}

  publish_failed()
    : mosquittopp_error(msg) {}
};

struct mosquitto_handle_deleter {
  void operator()(mosq_c::mosquitto* mosq) const {
    if (mosq != nullptr) {
      mosq_c::mosquitto_destroy(mosq);
    }
  }
};

using mosquitto_handle_base =
  std::unique_ptr<mosq_c::mosquitto, mosquitto_handle_deleter>;
class mosquitto_handle
  : public mosquitto_handle_base
{
public:
  using mosquitto_handle_base::mosquitto_handle_base;

  bool is_valid() const noexcept {
    return this->get() != nullptr;
  }

  operator mosq_c::mosquitto*() const noexcept {
    return this->get();
  }
};

using message = mosq_c::mosquitto_message;
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
  message const* msg
)>;

class mosquittopp {
private:
  bbzl::utils::net::openssl_ctx_handle ssl_ctx_;
  on_connect_cb_fn on_connect_cb_;
  on_disconnect_cb_fn on_disconnect_cb_;
  on_suback_cb_fn on_suback_cb_;
  on_message_cb_fn on_message_cb_;
  mosquitto_handle mosq_;
  int keepalive_ = 0;
  std::atomic_bool connected_ = false;
  std::string active_addr_;
  uint16_t active_port_ = 0;
  void thread_main(
    std::stop_token&& st,
    std::string const&& addr,
    uint16_t port
  );
  std::jthread thread_;
  std::atomic_bool thread_active_ = false;

  friend void on_connect(
    mosq_c::mosquitto*,
    void* obj,
    int reason_code
  ) noexcept;
  friend void on_disconnect(
    mosq_c::mosquitto*,
    void* obj,
    int reason_code
  ) noexcept;
  friend void on_message(
    mosq_c::mosquitto*,
    void* obj,
    mosq_c::mosquitto_message const* msg
  ) noexcept;
  friend void on_suback(
    mosq_c::mosquitto*,
    void* obj,
    int mid,
    int qos_count,
    int const* granted_qos
  ) noexcept;

public:
  mosquittopp();
  ~mosquittopp();
  void set_on_connect_cb(on_connect_cb_fn&& fn);
  void set_on_disconnect_cb(on_disconnect_cb_fn&& fn);
  void set_on_suback_cb(on_suback_cb_fn&& fn);
  void set_on_message_cb(on_message_cb_fn&& fn);
  [[nodiscard]]
  bool active() const noexcept;
  [[nodiscard]]
  bool connected() const noexcept;
  void connect(
    std::string const& addr,
    uint16_t port,
    std::string const& username,
    std::string const& password,
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

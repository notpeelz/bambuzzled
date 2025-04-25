#pragma once

#include <memory>
#include <thread>

#include <openssl/ssl.h>
#include <mosquitto.h>

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

class mosquittopp {
private:
  openssl_ctx_ptr openssl_ctx;
  mosquitto_ptr mosq;
  int keepalive = 0;
  void thread_main(std::stop_token st);
  std::jthread thread;
public:
  mosquittopp(void* obj);
  ~mosquittopp();
  mosquitto* get();
  void connect(
    std::string& addr,
    uint16_t port,
    std::string& username,
    std::string& password,
    int keepalive = 0
  );
  void disconnect();
};

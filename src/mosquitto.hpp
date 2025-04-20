#pragma once

#include <memory>

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

class mosquitto {
private:
  openssl_ctx_ptr openssl_ctx;
  mosquitto_ptr mosq;
  bool started;
public:
  mosquitto(void* obj);
  ~mosquitto();
  mosquitto* get();
  void start();
  void stop();
};

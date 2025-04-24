#include <string>
#include <memory>
#include <mutex>
#include <stdexcept>

#include <openssl/err.h>

#include "mosquittopp.hpp"

std::string get_openssl_err() {
  BIO* bio = BIO_new(BIO_s_mem());
  ERR_print_errors(bio);
  char* buf;
  size_t len = BIO_get_mem_data(bio, &buf);
  std::string s(buf, len);
  BIO_free(bio);
  return s;
}

openssl_ctx_ptr create_openssl_context() {
  const SSL_METHOD* method = TLS_client_method();
  auto ctx = openssl_ctx_ptr(SSL_CTX_new(method));

  if (!ctx) {
    throw std::runtime_error("failed to create OpenSSL context: " + get_openssl_err());
  }

  SSL_CTX_set_verify(ctx.get(), SSL_VERIFY_PEER, nullptr);

  X509_STORE* store = SSL_CTX_get_cert_store(ctx.get());
  X509_STORE_set_flags(store, X509_V_FLAG_NO_CHECK_TIME);

  // TODO: embed this file or make it packageable/configurable?
  if (SSL_CTX_load_verify_file(ctx.get(), "/home/peelz/code/3dp/re/bambuzzled/blcert.pem") != 1) {
    throw std::runtime_error("failed to load CA certificate(s): " + get_openssl_err());
  }

  SSL_CTX_set_min_proto_version(ctx.get(), TLS1_2_VERSION);

  return ctx;
}

mosquittopp::mosquittopp(void* userdata) {
  int rc;

  static std::once_flag once;
  std::call_once(once, []() {
    mosquitto_lib_init();
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
  });

  this->mosq = mosquitto_ptr(mosquitto_new(nullptr, true, userdata));
  if (this->mosq == nullptr) {
    throw std::runtime_error("failed to create Mosquitto instance");
  }

  this->openssl_ctx = create_openssl_context();
  rc = mosquitto_int_option(this->mosq.get(), MOSQ_OPT_SSL_CTX_WITH_DEFAULTS, 0);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(
      "failed to disable default Mosquitto OpenSSL context: "
      + std::string(mosquitto_strerror(rc))
    );
  }

  rc = mosquitto_void_option(this->mosq.get(), MOSQ_OPT_SSL_CTX, this->openssl_ctx.get());
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(
      "failed to set Mosquitto OpenSSL context: "
      + std::string(mosquitto_strerror(rc))
    );
  }

  rc = mosquitto_int_option(this->mosq.get(), MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V31);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(
      "failed to set Mosquitto protocol version: "
      + std::string(mosquitto_strerror(rc))
    );
  }
}

mosquittopp::~mosquittopp() {
  (void)mosquitto_disconnect(this->mosq.get());
  if (this->started) {
    (void)mosquitto_loop_stop(this->mosq.get(), false);
  }
}

mosquitto* mosquittopp::get() {
  return this->mosq.get();
}

void mosquittopp::start() {
  if (this->started) {
    throw std::runtime_error("Mosquitto thread was already started");
  }

  int rc = mosquitto_loop_start(this->mosq.get());
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(
      "failed to start Mosquitto thread: "
      + std::string(mosquitto_strerror(rc))
    );
  }
  this->started = true;
}

void mosquittopp::stop() {
  if (!this->started) {
    throw std::runtime_error("Mosquitto thread is not running");
  }

  (void)mosquitto_disconnect(this->mosq.get());
  (void)mosquitto_loop_stop(this->mosq.get(), false);

  this->started = false;
}

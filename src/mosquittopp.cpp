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

  rc = mosquitto_threaded_set(this->mosq.get(), true);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(
      "failed to enable Mosquitto threaded mode: "
      + std::string(mosquitto_strerror(rc))
    );
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
  this->disconnect();
}

mosquitto* mosquittopp::get() {
  return this->mosq.get();
}

void mosquittopp::thread_main(std::stop_token st) {
  int rc;
  int err;
  auto mosq = this->mosq.get();

  // sleep for a day if keepalive is disabled
  int timeout = 1000 * 86400;
  if (this->keepalive > 0) {
    timeout = this->keepalive * 1000;
  }

#if !defined(WIN32)
  timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 10000000;
#endif

  while (true) {
    rc = mosquitto_loop(mosq, timeout, 1);
    if (rc != MOSQ_ERR_NO_CONN) {
      break;
    }

#if defined(WIN32)
    Sleep(10);
#else
    nanosleep(&ts, NULL);
#endif

  }

  while (true) {
    do {
      rc = mosquitto_loop(mosq, timeout, 1);
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

  this->thread = std::jthread([&](std::stop_token st) {
    this->thread_main(st);
  });

  auto mosq = this->mosq.get();

  rc = mosquitto_username_pw_set(mosq, username.c_str(), password.c_str());
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error("failed to set MQTT credentials");
  }

  rc = mosquitto_connect(mosq, addr.c_str(), port, keepalive);
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(
      "failed to connect to MQTT broker: "
      + std::string(mosquitto_strerror(rc))
    );
  }

  this->keepalive = keepalive;
}

void mosquittopp::disconnect() {
  if (!this->thread.joinable()) {
    return;
  }

  int rc = mosquitto_disconnect(this->mosq.get());
  if (rc != MOSQ_ERR_SUCCESS) {
    throw std::runtime_error(
      "failed to request disconnect: "
      + std::string(mosquitto_strerror(rc))
    );
  }

  (void)this->thread.get_stop_source().request_stop();
}

#include <mosquitto.h>
#include <iostream>
#include <cstring>
#include <mosquitto.h>
#include <csignal>
#include <thread>
#include <openssl/ssl.h>
#include <openssl/err.h>

// Thanks Claude

// Global mosquitto client instance
struct mosquitto *mosq = NULL;

// Signal handler for clean termination
void signal_handler(int signum) {
  std::cout << "Received signal " << signum << ", cleaning up..." << std::endl;
  if (mosq) {
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
  }
  mosquitto_lib_cleanup();
  exit(signum);
}

// Callback for connection status
void on_connect(struct mosquitto *mosq, void *obj, int reason_code) {
  if (reason_code != 0) {
    std::cerr << "Error connecting to broker: " << mosquitto_connack_string(reason_code) << std::endl;
    return;
  }

  std::cout << "Connected to MQTT broker successfully" << std::endl;

  // Subscribe to the topic after successful connection
  const char* topic = "device/serial-goes-here/report";
  int qos = 1; // QoS level 1 (at least once)

  int rc = mosquitto_subscribe(mosq, NULL, topic, qos);
  if (rc != MOSQ_ERR_SUCCESS) {
    std::cerr << "Error subscribing to topic: " << mosquitto_strerror(rc) << std::endl;
  } else {
    std::cout << "Subscribed to topic: " << topic << std::endl;
  }
}

// Callback for message reception
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
  std::cout << "Received message on topic: " << message->topic << std::endl;
  std::cout << "Message: " << std::string((char*)message->payload, message->payloadlen) << std::endl;
}

// Callback for disconnection
void on_disconnect(struct mosquitto *mosq, void *obj, int reason_code) {
  std::cout << "Disconnected from broker" << std::endl;
  if (reason_code != 0) {
    std::cerr << "Unexpected disconnect: " << mosquitto_strerror(reason_code) << std::endl;
  }
}

int custom_verify_callback(int preverify_ok, X509_STORE_CTX* ctx) {
  // X509* x509 = X509_STORE_CTX_get_current_cert(ctx);
  // X509_NAME* name = X509_get_issuer_name(x509);

  // char name_buf[4096];
  // X509_NAME_oneline(name, name_buf, sizeof(name_buf));
  // std::cerr << "test: " << name_buf << std::endl;

  if (preverify_ok == 1) {
    return 1;
  }

  int err = X509_STORE_CTX_get_error(ctx);
  int depth = X509_STORE_CTX_get_error_depth(ctx);

  std::cerr << "Certificate verification error at depth: " << depth << std::endl;
  std::cerr << "Error: " << X509_verify_cert_error_string(err) << std::endl;

  return 0;
}

// Function to create and configure an SSL context
SSL_CTX* create_custom_ssl_context() {
  // Initialize OpenSSL
  SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();

  // Create a new SSL context
  const SSL_METHOD* method = TLS_client_method();
  SSL_CTX* ctx = SSL_CTX_new(method);

  if (!ctx) {
    std::cerr << "Error creating SSL context" << std::endl;
    ERR_print_errors_fp(stderr);
    return nullptr;
  }

  // Set the verification mode
  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, custom_verify_callback);

  X509_STORE* store = SSL_CTX_get_cert_store(ctx);
  X509_STORE_set_flags(store, X509_V_FLAG_NO_CHECK_TIME);

  // Load trusted CA certificates
  if (SSL_CTX_load_verify_file(ctx, "blcert.pem") != 1) {
    std::cerr << "Error loading CA certificate" << std::endl;
    ERR_print_errors_fp(stderr);
    SSL_CTX_free(ctx);
    return nullptr;
  }

  // Set minimum TLS version if needed
  SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

  // You can add more SSL_CTX configurations here

  return ctx;
}

int main() {
  int rc;

  // Set up signal handling for clean termination
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  // Initialize mosquitto library
  mosquitto_lib_init();

  // Create a client instance
  const char* client_id = "cpp_subscriber"; // Client ID
  bool clean_session = true;

  mosq = mosquitto_new(client_id, clean_session, NULL);
  if (!mosq) {
    std::cerr << "Error creating mosquitto client instance" << std::endl;
    return 1;
  }

  // Create custom SSL context
  SSL_CTX* ssl_ctx = create_custom_ssl_context();
  if (!ssl_ctx) {
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 1;
  }

  // Set the SSL context option for mosquitto
  rc = mosquitto_int_option(mosq, MOSQ_OPT_SSL_CTX_WITH_DEFAULTS, 0);
  if (rc != MOSQ_ERR_SUCCESS) {
    std::cerr << "Error disabling default SSL context: " << mosquitto_strerror(rc) << std::endl;
    SSL_CTX_free(ssl_ctx);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 1;
  }

  rc = mosquitto_void_option(mosq, MOSQ_OPT_SSL_CTX, ssl_ctx);
  if (rc != MOSQ_ERR_SUCCESS) {
    std::cerr << "Error setting SSL context: " << mosquitto_strerror(rc) << std::endl;
    SSL_CTX_free(ssl_ctx);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 1;
  }

  rc = mosquitto_int_option(mosq, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V31);
  if (rc != MOSQ_ERR_SUCCESS) {
    std::cerr << "Error setting MQTT protocol version: " << mosquitto_strerror(rc) << std::endl;
    SSL_CTX_free(ssl_ctx);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 1;
  }

  constexpr char* username = "bblp";
  constexpr char* password = "password-goes-here";
  mosquitto_username_pw_set(mosq, username, password);

  mosquitto_connect_callback_set(mosq, on_connect);
  mosquitto_message_callback_set(mosq, on_message);
  mosquitto_disconnect_callback_set(mosq, on_disconnect);

  constexpr char* host = "ip-goes-here";
  constexpr int port = 8883;
  constexpr int keepalive = 60;

  rc = mosquitto_connect(mosq, host, port, keepalive);
  if (rc != MOSQ_ERR_SUCCESS) {
    std::cerr << "Error connecting to broker: " << mosquitto_strerror(rc) << std::endl;
    SSL_CTX_free(ssl_ctx);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 1;
  }

  // Start the network loop
  rc = mosquitto_loop_start(mosq);
  if (rc != MOSQ_ERR_SUCCESS) {
    std::cerr << "Error starting mosquitto network loop: " << mosquitto_strerror(rc) << std::endl;
    SSL_CTX_free(ssl_ctx);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 1;
  }

  std::cout << "MQTT client running. Press Ctrl+C to exit." << std::endl;

  // Keep the program running
  while (true) {
    // You can add additional program logic here if needed
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // Clean up (this will only execute if the while loop is broken, which won't happen with the current logic)
  mosquitto_loop_stop(mosq, true);
  mosquitto_disconnect(mosq);
  SSL_CTX_free(ssl_ctx);
  mosquitto_destroy(mosq);
  mosquitto_lib_cleanup();

  return 0;
}

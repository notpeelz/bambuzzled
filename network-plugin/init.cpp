#include <openssl/ssl.h>
#include <mosquitto.h>
#include <curl/curl.h>

static void init() {
  int rc;

  rc = OPENSSL_init_ssl(
    OPENSSL_INIT_LOAD_SSL_STRINGS
    | OPENSSL_INIT_LOAD_CRYPTO_STRINGS,
    nullptr
  );
  if (rc != 1) {
    abort();
  }

  rc = OPENSSL_init_crypto(
    OPENSSL_INIT_ADD_ALL_CIPHERS
    | OPENSSL_INIT_ADD_ALL_DIGESTS,
    nullptr
  );
  if (rc != 1) {
    abort();
  }

  rc = mosquitto_lib_init();
  if (rc != MOSQ_ERR_SUCCESS) {
    abort();
  }

  rc = curl_global_init(CURL_GLOBAL_DEFAULT);
  if (rc != CURLE_OK) {
    abort();
  }
}

static void cleanup() {
  mosquitto_lib_cleanup();
  OPENSSL_cleanup();
}

#if defined(_WIN32)

#include <windows.h>

BOOL WINAPI DllMain(
  HINSTANCE instance,
  DWORD reason,
  LPVOID reserved
) {
  (void)reserved;
  if (reason == DLL_PROCESS_ATTACH) {
    init();
  }

  if (reason == DLL_PROCESS_DETACH) {
    cleanup();
  }

  return true;
}

#else

[[gnu::constructor]]
static void ctor() {
  init();
}

[[gnu::destructor]]
static void dtor() {
  cleanup();
}

#endif

#include <span>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include <format>
#include <nlohmann/json.hpp>
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/prov_ssl.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/socket.hpp>
#include <bambuzzled/socket_encrypted.hpp>
#include <bambuzzled/networking/api/bambu_networking.hpp>
#include "pairing.hpp"

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

using json = nlohmann::json;
using bbzl::ssl::openssl_ctx_handle;

namespace bbzl::pairing {

static bool parse_ip(std::string const& s, sockaddr_storage& sa) {
  sa = {};

  auto& sa_in6 = reinterpret_cast<sockaddr_in6&>(sa);

  if (inet_pton(AF_INET6, s.c_str(), &sa_in6.sin6_addr) == 1) {
    sa_in6.sin6_family = AF_INET6;
    return true;
  }

  auto& sa_in4 = reinterpret_cast<sockaddr_in&>(sa);
  if (inet_pton(AF_INET, s.c_str(), &sa_in4.sin_addr) == 1) {
    sa_in4.sin_family = AF_INET;
    return true;
  }

  return false;
}

static openssl_ctx_handle create_openssl_context() {
  auto method = TLS_client_method();
  auto ctx = openssl_ctx_handle(SSL_CTX_new(method));
  SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

  if (ctx == nullptr) {
    throw std::runtime_error(std::format(
      "failed to create OpenSSL context: {}",
      bbzl::ssl::get_err_msg()
    ));
  }

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
      bbzl::ssl::get_certs_dir().u8string().c_str()
    ),
    X509_FILETYPE_PEM
  );

  return ctx;
}

void discover(
  std::string const& addr,
  bool use_ssl,
  BBL::detectResult& result
) {
  using utils::socket::socket;
  using utils::socket::socket_encrypted;

  int r;
  union {
    sockaddr sa;
    sockaddr_in s4;
    sockaddr_in6 s6;
    sockaddr_storage ss;
  } ss;
  if (!parse_ip(addr, ss.ss)) {
    throw std::runtime_error("invalid address");
  }

  #if defined(_WIN32)
  USHORT* ss_port = nullptr;
  #else
  in_port_t* ss_port = nullptr;
  #endif

  switch (ss.sa.sa_family) {
    case AF_INET: {
      ss_port = &ss.s4.sin_port;
      break;
    }
    case AF_INET6: {
      ss_port = &ss.s6.sin6_port;
      break;
    }
    default: {
      std::abort();
      break;
    }
  }

  openssl_ctx_handle ssl_ctx;
  auto sock_variant = std::variant<socket, socket_encrypted>();
  if (use_ssl) {
    *ss_port = ::htons(3002);
    ssl_ctx = create_openssl_context();
    sock_variant.emplace<1>(ssl_ctx);
  } else {
    *ss_port = ::htons(3000);
    sock_variant.emplace<0>();
  }
  auto& sock = std::visit(
    [](auto& s) -> socket& {
      return s;
    },
    sock_variant
  );

  sock.connect(ss.sa, sizeof(ss));

  constexpr int timeout_seconds = 5;
  sock.set_recv_timeout(timeout_seconds);
  sock.set_send_timeout(timeout_seconds);

  auto request_json = R"({
    "login": {
      "command": "detect",
      "sequence_id": "50000"
    }
  })"_json;

  auto data = std::vector<std::byte>();
  data.push_back(std::byte{0xa5});
  data.push_back(std::byte{0xa5});
  data.push_back(std::byte{0x3a});
  data.push_back(std::byte{0x00});
  for (auto& c : request_json.dump()) {
    data.push_back(static_cast<std::byte>(c));
  }
  data.push_back(std::byte{0xa7});
  data.push_back(std::byte{0xa7});

  constexpr static auto end_pattern = {std::byte(0xa7), std::byte(0xa7)};

  sock.send(data);

  bool found_end = false;
  auto buf = std::vector<std::byte>(0x800);

  size_t offset = 0;
  while (true) {
    auto const msg_end = std::search(
      buf.cbegin(),
      buf.cend(),
      end_pattern.begin(),
      end_pattern.end()
    );

    if (msg_end != buf.end()) {
      found_end = true;
      buf.erase(msg_end + end_pattern.size(), buf.cend());
      break;
    }

    if (offset >= buf.size()) {
      buf.resize(buf.size() + 0x800);
    }

    if (buf.size() > 0x10000) {
      throw std::runtime_error("message received is too large");
    }

    auto buf_span = std::span<std::byte>(
      buf.begin() + offset,
      buf.end()
    );
    r = sock.receive(buf_span);
    if (r == 0) {
      break;
    }
    offset += r;
  }

  if (!found_end) {
    throw std::runtime_error("received incomplete message");
  }

  constexpr size_t header_size = 4;
  constexpr size_t trailer_size = end_pattern.size();
  if (buf.size() < (header_size + trailer_size)) {
    throw std::runtime_error("received message too small to parse");
  }

  auto byte_span = std::span<std::byte const>(
    buf.cbegin() + header_size,
    buf.cend() - trailer_size
  );
  auto s = std::string(buf.size(), '\0');
  auto s_end = std::transform(
    byte_span.begin(),
    byte_span.end(),
    s.begin(),
    [](std::byte b) {
      return static_cast<char>(b);
    }
  );
  s.erase(s_end, s.cend());
  result.result_msg = s;

  auto json = json::parse(s);
  auto& login = json["login"];
  result.command = login["command"];
  result.dev_id = login["id"];
  result.model_id = login["model"];
  result.dev_name = login["name"];
  result.version = login["version"];
  result.bind_state = login["bind"];
  result.connect_type = login["connect"];
}

}

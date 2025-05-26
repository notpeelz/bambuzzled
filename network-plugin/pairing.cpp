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
#include <bambuzzled/log.hpp>
#include <bambuzzled/ssl.hpp>
#include <bambuzzled/socket.hpp>
#include <bambuzzled/socket_encrypted.hpp>
#include "pairing.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

using json = nlohmann::json;
using bbzl::utils::net::openssl_ctx_handle;

namespace bbzl::pairing {

template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

static openssl_ctx_handle create_openssl_context() {
  auto method = TLS_client_method();
  auto ctx = openssl_ctx_handle(SSL_CTX_new(method));
  SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

  if (ctx == nullptr) {
    throw std::runtime_error(std::format(
      "failed to create ssl context: {}",
      bbzl::utils::net::get_ssl_error_messages()
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
      bbzl::utils::net::get_certs_dir().u8string().c_str()
    ),
    X509_FILETYPE_PEM
  );

  return ctx;
}

using any_addr = std::variant<in_addr, in6_addr>;

static std::string probe_binding_info(
  any_addr addr,
  bbzl::utils::net::net_port port,
  bool use_ssl
) {
  using bbzl::utils::net::socket;
  using bbzl::utils::net::socket_encrypted;

  int r;
  openssl_ctx_handle ssl_ctx;
  utils::net::net_port port_network_order = ::htons(port);
  auto sock_variant = std::variant<socket, socket_encrypted>();
  if (use_ssl) {
    ssl_ctx = create_openssl_context();
    sock_variant.emplace<1>(ssl_ctx);
  } else {
    sock_variant.emplace<0>();
  }
  auto& sock = std::visit(
    [](auto& s) -> socket& {
      return s;
    },
    sock_variant
  );

  std::visit(
    overloads {
      [&](in_addr& addr) {
        sock.connect(sockaddr_in {
          .sin_family = AF_INET,
          .sin_port = port_network_order,
          .sin_addr = addr,
        });
      },
      [&](in6_addr& addr) {
        sock.connect(sockaddr_in6 {
          .sin6_family = AF_INET6,
          .sin6_port = port_network_order,
          .sin6_addr = addr,
        });
      },
    },
    addr
  );

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
      throw std::runtime_error("received message is too large");
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
    throw std::runtime_error("received message is too small to parse");
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

  return s;
}

std::string probe_binding_info(
  in_addr addr,
  bbzl::utils::net::net_port port,
  bool use_ssl
) {
  return probe_binding_info(any_addr(addr), port, use_ssl);
}

std::string probe_binding_info(
  in6_addr addr,
  bbzl::utils::net::net_port port,
  bool use_ssl
) {
  return probe_binding_info(any_addr(addr), port, use_ssl);
}

}

#include <cerrno>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <format>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <nlohmann/json.hpp>
#include <bambuzzled/fd_handle.hpp>
#include <bambuzzled/networking/api/bambu_networking.hpp>

#include "pairing.hpp"

using json = nlohmann::json;

namespace bbzl::pairing {

bool parse_ip(std::string const& s, sockaddr_storage& sa) {
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

void discover(
  std::string const& addr,
  bool use_ssl,
  BBL::detectResult& result
) {
  int r;

  union {
    sockaddr sa;
    sockaddr_in s4;
    sockaddr_in6 s6;
    sockaddr_storage ss;
  } sa;

  if (!parse_ip(addr, sa.ss)) {
    throw std::runtime_error("invalid address");
  }

  auto sa_family = sa.sa.sa_family;
  in_port_t* port = nullptr;
  bbzl::utils::fd_handle sockfd;
  if (sa_family == AF_INET) {
    sockfd = bbzl::utils::fd_handle(
      socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
    );
    port = &sa.s4.sin_port;
  } else if (sa_family == AF_INET6) {
    sockfd = bbzl::utils::fd_handle(
      socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)
    );
    port = &sa.s6.sin6_port;
  } else {
    abort();
  }

  timeval tv = {
    .tv_sec = 5,
  };
  r = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  if (r < 0) {
    throw std::runtime_error(std::format(
      "failed to set SO_RCVTIMEO: {}",
      strerror(errno)
    ));
  }

  r = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
  if (r < 0) {
    throw std::runtime_error(std::format(
      "failed to set SO_SNDTIMEO: {}",
      strerror(errno)
    ));
  }

  // TODO: do OpenSSL stuff if use_ssl
  *port = htons(3000);

  if (sockfd < 0) {
    auto err = std::strerror(errno);
    throw std::runtime_error(std::format(
      "failed to create socket: {}",
      err
    ));
  }

  r = connect(sockfd, &sa.sa, sizeof(sa));
  if (r < 0) {
    throw std::runtime_error(std::format(
      "can't connect: {}",
      strerror(errno)
    ));
  }

  auto json = R"({
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
  for (auto& c : json.dump()) {
    data.push_back(static_cast<std::byte>(c));
  }
  data.push_back(std::byte{0xa7});
  data.push_back(std::byte{0xa7});

  static auto const end_pattern = {std::byte(0xa7), std::byte(0xa7)};

  while (true) {
    r = write(sockfd, data.data(), data.size());
    if (r < 0) {
      if (errno == EINTR) {
        continue;
      }

      throw std::runtime_error(std::format(
        "failed to send message: {}",
        strerror(errno)
      ));
    }

    break;
  }

  bool found_end = false;
  auto buf = std::vector<std::byte>();
  buf.resize(0x800);

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

    r = recv(sockfd, buf.data() + offset, buf.size() - offset, 0);
    if (r < 0) {
      if (errno == EINTR) {
        continue;
      }

      throw std::runtime_error(std::format(
        "failed to receive message: {}",
        strerror(errno)
      ));
    }

    if (r == 0) {
      break;
    }

    offset += r;
  }

  if (found_end) {
    auto s = std::string(buf.cbegin() + 4, buf.cend() - 2);
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

}

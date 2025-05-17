#pragma once

#include <span>
#include <cstddef>
#include <sys/socket.h>
#include <netinet/in.h>
#include <bambuzzled/socket_handle.hpp>

namespace bbzl::utils::socket {

class socket {
protected:
  bbzl::utils::socket::socket_handle handle_;

public:
  socket() = default;
  virtual ~socket() = default;

  void set_recv_timeout(int seconds);
  void set_send_timeout(int seconds);

  virtual void connect(sockaddr const& addr, size_t addr_len);
  void connect(sockaddr_in const& addr) {
    this->connect(
      *reinterpret_cast<sockaddr const*>(&addr),
      sizeof(addr)
    );
  }
  void connect(sockaddr_in6 const& addr) {
    this->connect(
      *reinterpret_cast<sockaddr const*>(&addr),
      sizeof(addr)
    );
  }

  virtual int send(std::span<std::byte> const& data);
  virtual int receive(std::span<std::byte>& buf);
};

}

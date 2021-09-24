#pragma once

#include "asio.hpp"
#include "base/noncopyable.hpp"

using asio::ip::tcp;

class TcpSession : public std::enable_shared_from_this<TcpSession> {
  friend class TcpServer;

 public:
  explicit TcpSession(tcp::socket socket, const uint& buffer_len);
  bool send(const void* data, size_t len);
  std::function<void()> onClose;
  std::function<void(uint8_t* data, size_t len)> onData;

 private:
  void do_read();
  void do_close();

 private:
  tcp::socket socket_;
  const uint& buffer_len_;
  std::string buffer_;
};

class TcpServer {
 public:
  TcpServer(asio::io_context& io_context, uint16_t port,
            uint buffer_len = 1024);
  std::function<void(std::weak_ptr<TcpSession>)> onNewSession;

 private:
  void do_accept();

 private:
  tcp::acceptor acceptor_;
  uint buffer_len_;
};

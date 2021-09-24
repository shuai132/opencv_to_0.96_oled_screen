#include "TcpServer.h"

TcpSession::TcpSession(tcp::socket socket, const uint& buffer_len)
    : socket_(std::move(socket)), buffer_len_(buffer_len) {
  buffer_.reserve(buffer_len_);
}

bool TcpSession::send(const void* data, size_t len) {
  asio::async_write(
      socket_, asio::buffer(data, len),
      [this, payloadSize = len](std::error_code ec, std::size_t length) {
        if (ec || length != payloadSize) {
          do_close();
        }
      });
  return true;
}

void TcpSession::do_read() {
  auto self(shared_from_this());
  socket_.async_read_some(asio::buffer((void*)buffer_.data(), buffer_len_),
                          [this, self](std::error_code ec, std::size_t length) {
                            if (!ec) {
                              if (onData) {
                                onData((uint8_t*)buffer_.data(), length);
                              }
                              do_read();
                            } else {
                              do_close();
                            }
                          });
}

void TcpSession::do_close() {
  socket_.close();
  if (onClose) {
    onClose();
  }
}

TcpServer::TcpServer(asio::io_context& io_context, uint16_t port,
                     uint buffer_len)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      buffer_len_(buffer_len) {
  do_accept();
}

void TcpServer::do_accept() {
  acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
    if (!ec) {
      auto session =
          std::make_shared<TcpSession>(std::move(socket), buffer_len_);
      session->do_read();
      if (onNewSession) onNewSession(session);
    }

    do_accept();
  });
}

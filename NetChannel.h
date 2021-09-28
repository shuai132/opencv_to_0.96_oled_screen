#pragma once

#include "noncopyable.hpp"
#include "tcp_server.hpp"

using namespace asio_tcp;

class NetChannel : noncopyable {
public:
    explicit NetChannel(asio::io_context* context, uint16_t port);
    void sendData(std::string data);
    std::function<void(std::weak_ptr<tcp_session>)> onNewSession;
    std::function<void(std::weak_ptr<tcp_session>, std::string data)> onData;
    std::function<void(std::weak_ptr<tcp_session>)> onClose;

private:
    asio::io_context* context_;
    tcp_server server_;
    std::weak_ptr<tcp_session> session_;
};

#pragma once

#include "noncopyable.hpp"
#include "tcp_server.hpp"

using namespace asio_tcp;

class NetChannel : noncopyable {
public:
    explicit NetChannel(asio::io_context* context);
    void sendData(std::string data);
    std::function<void(std::string data)> onData;

private:
    asio::io_context* context_;
    tcp_server server_;
    std::weak_ptr<tcp_session> session_;
};

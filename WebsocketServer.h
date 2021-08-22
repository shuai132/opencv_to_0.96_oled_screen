#pragma once

#include "noncopyable.hpp"
#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

class WebsocketServer : noncopyable {
public:
    explicit WebsocketServer(asio::io_context* context);
    ~WebsocketServer();

    void start();

    /// only one hdl
    void sendData(void* data, size_t len);
    std::function<void(void* data, size_t len)> onData;

private:
    asio::io_context* context_;
    websocketpp::server<websocketpp::config::asio> server_;
    websocketpp::connection_hdl hdl_;
};

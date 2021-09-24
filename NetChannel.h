#pragma once

#include "noncopyable.hpp"
#include "TcpServer.h"
#include "PacketProcessor.h"

class NetChannel : noncopyable {
public:
    explicit NetChannel(asio::io_context* context);
    void sendData(void* data, size_t len);
    std::function<void(void* data, size_t len)> onData;

private:
    asio::io_context* context_;
    TcpServer server_;
    std::weak_ptr<TcpSession> session_;
    PacketProcessor packetProcessor_;
};

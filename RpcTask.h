#pragma once

#include "noncopyable.hpp"
#include "asio.hpp"
#include "NetChannel.h"
#include "RpcCore.hpp"
#include "AsioContext.h"
#include "udp_multicast.hpp"
#include "udp_client.hpp"

class RpcTask : public AsioContext {
public:
    RpcTask();
    ~RpcTask();

    void initRpc();

    void onFrame(void* data, int size);

    void waitConnect();

    std::shared_ptr<RpcCore::Rpc> rpc();

private:
    std::unique_ptr<NetChannel> server_;
    std::unique_ptr<std::thread> thread_;

    std::shared_ptr<RpcCore::Rpc> rpc_;
    std::unique_ptr<udp_multicast::sender> udp_multicast_sender_;
    const uint16_t tcp_socket_port_ = 3000;
    const uint16_t udp_socket_port_ = 3001;
    std::unique_ptr<asio_udp::udp_client> udp_client_;
    std::unique_ptr<asio::ip::udp::endpoint> udp_remote_endpoint_;
};

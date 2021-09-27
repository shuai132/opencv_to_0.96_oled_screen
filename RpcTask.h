#pragma once

#include "noncopyable.hpp"
#include "asio.hpp"
#include "NetChannel.h"
#include "RpcCore.hpp"
#include "AsioContext.h"
#include "udp_multicast.hpp"

class RpcTask : public AsioContext {
public:
    RpcTask();
    ~RpcTask();

    void initRpc();

    void onFrame(void* data, int size);

private:
    std::unique_ptr<NetChannel> server_;
    std::unique_ptr<std::thread> thread_;

    std::shared_ptr<RpcCore::Rpc> rpc_;
    std::unique_ptr<udp_multicast::sender> udp_multicast_sender_;
    std::unique_ptr<asio::ip::udp::socket> udp_socket_;
    std::string udp_remote_ip_;
};

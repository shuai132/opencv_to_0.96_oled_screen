#pragma once

#include "noncopyable.hpp"
#include "asio.hpp"
#include "WebsocketServer.h"
#include "RpcCore.hpp"
#include "AsioContext.h"

class RpcTask : public AsioContext {
public:
    RpcTask();
    ~RpcTask();

    void initRpc();

    void onFrame(void* data, int size);

private:
    std::unique_ptr<WebsocketServer> server_;
    std::unique_ptr<std::thread> thread_;

    std::shared_ptr<RpcCore::Rpc> rpc_;
};
#include "RpcTask.h"
#include "RpcCore.hpp"

RpcTask::RpcTask() {
    server_ = std::make_unique<NetChannel>(context_);
    thread_ = std::make_unique<std::thread>([this]{
        initRpc();
        context_->run();
    });
}

RpcTask::~RpcTask() {
    context_->stop();
    thread_->join();
}

void RpcTask::initRpc() {
    using namespace RpcCore;

    auto connection = std::make_shared<Connection>();
    connection->sendPackageImpl = [&](const std::string& package) {
        server_->sendData((void*)package.data(), package.length());
    };
    server_->onData = [connection](void* data, size_t len) {
        connection->onRecvPackage(std::string((char*)data, len));
    };

    rpc_ = Rpc::create(connection);
    rpc_->setTimer([this](uint32_t ms, const Rpc::TimeoutCb &cb) {
        postDelayed(cb, ms);
    });

    std::string uri = utils::get_ip_address()[0].ip + ":3000";
    udp_multicast_sender_ = std::make_unique<udp_multicast::sender>(*context_, "opencv_oled", std::move(uri));
}

void RpcTask::onFrame(void* data, int size) {
    using namespace RpcCore;
    Binary img((char*)data, size);
    context_->dispatch([this, img = std::move(img)]{
        rpc_->createRequest()
        ->cmd("img")
        ->msg(img)
        ->noRsp()
        ->call();
    });
}

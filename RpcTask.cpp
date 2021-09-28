#include "RpcTask.h"
#include "RpcCore.hpp"
#include "log.h"

RpcTask::RpcTask() {
    server_ = std::make_unique<NetChannel>(context_, tcp_socket_port_);
    udp_client_ = std::make_unique<asio_udp::udp_client>(*context_);
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
    connection->sendPackageImpl = [&](std::string package) {
        server_->sendData(std::move(package));
    };
    server_->onData = [connection](const std::weak_ptr<tcp_session>& ws, std::string data) {
        connection->onRecvPackage(std::move(data));
    };

    server_->onNewSession = [this](const std::weak_ptr<tcp_session>& ws) {
        auto remoteIp = ws.lock()->remote_endpoint().address().to_v4().to_string();
        LOGI("onNewSession: %s", remoteIp.c_str());
        auto endpoints = asio::ip::udp::endpoint(asio::ip::address_v4::from_string(remoteIp), udp_socket_port_);
        udp_remote_endpoint_ = std::make_unique<asio::ip::udp::endpoint>(endpoints);
    };
    server_->onClose = [this](const std::weak_ptr<tcp_session>& ws) {
        udp_remote_endpoint_.reset();
    };

    rpc_ = Rpc::create(connection);
    rpc_->setTimer([this](uint32_t ms, const Rpc::TimeoutCb &cb) {
        postDelayed(cb, ms);
    });

    std::string uri = utils::get_ip_address()[0].ip + ":" + std::to_string(tcp_socket_port_);
    udp_multicast_sender_ = std::make_unique<udp_multicast::sender>(*context_, "opencv_oled", std::move(uri));
}

void RpcTask::onFrame(void* data, int size) {
    if (!udp_remote_endpoint_) return;
    udp_client_->do_send(data, size, *udp_remote_endpoint_);
}

void RpcTask::waitConnect(){
    for(;;) {
        if (udp_remote_endpoint_) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

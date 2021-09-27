#include "RpcTask.h"
#include "RpcCore.hpp"

using asio::ip::udp;

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
    connection->sendPackageImpl = [&](std::string package) {
        server_->sendData(std::move(package));
    };
    server_->onData = [connection](std::string data) {
        connection->onRecvPackage(std::move(data));
    };

    rpc_ = Rpc::create(connection);
    rpc_->setTimer([this](uint32_t ms, const Rpc::TimeoutCb &cb) {
        postDelayed(cb, ms);
    });
    rpc_->subscribe<String>("ready", [this](const String& ip){
        printf("wtf:ready: %s\n", ip.c_str());
        udp_socket_ = std::make_unique<udp::socket>(*context_, udp::endpoint(udp::v4(), 3001));
        udp_remote_ip_ = ip;
    });

    std::string uri = utils::get_ip_address()[0].ip + ":3000";
    udp_multicast_sender_ = std::make_unique<udp_multicast::sender>(*context_, "opencv_oled", std::move(uri));
}

void RpcTask::onFrame(void* data, int size) {
    if (!udp_socket_) return;
    auto keeper = std::make_unique<std::string>((char*)data, size);

    udp::resolver resolver(*context_);
    udp::resolver::results_type endpoints =
        resolver.resolve(udp::v4(), udp_remote_ip_, "3001");
//    udp_socket_->send_to(asio::buffer(request, request_length), *endpoints.begin());
    udp_socket_->async_send_to(
        asio::buffer(keeper->data(), keeper->length()), *endpoints.begin(),
        [keeper = std::move(keeper)](std::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
        });
}

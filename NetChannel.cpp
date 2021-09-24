#include "NetChannel.h"
#include "log.h"
#include "utils.h"

NetChannel::NetChannel(asio::io_context* context)
        : context_(context), server_(*context, 3000) {
    server_.onNewSession = [this](const std::weak_ptr<TcpSession>& hdl) {
        if (!session_.expired()) return;
        session_ = hdl;
        auto s = session_.lock();
        LOGD("connected: %p",s.get());
        s->onClose = [this, hdl]{
            LOGD("disconnected: %p", hdl.lock().get());
            session_.reset();
        };
        packetProcessor_.setOnPacketHandle([this](uint8_t* data, size_t size) {
            if (onData) onData(data, size);
        });
        s->onData = [this](uint8_t* data, size_t len) {
            packetProcessor_.feed(data, len);
        };
    };
}

void NetChannel::sendData(void* data, size_t len) {
    if (session_.expired()) return;
    auto packet = packetProcessor_.pack(data, len);
    session_.lock()->send(packet.data(), packet.size());
}

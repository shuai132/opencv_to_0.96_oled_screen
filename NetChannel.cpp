#include "NetChannel.h"
#include "log.h"
#include "utils.h"

NetChannel::NetChannel(asio::io_context* context, uint16_t port)
        : context_(context), server_(*context, port) {
    server_.onNewSession = [this](std::weak_ptr<tcp_session> ws) {
        if (!session_.expired()) {
            session_.lock()->close();
        };
        session_ = ws;
        auto s = session_.lock();
        LOGD("connected: %p",s.get());
        s->onClose = [this, ws]{
            LOGD("disconnected: %p", ws.lock().get());
            session_.reset();
            if (onClose) onClose(ws);
        };
        s->onData = [this, ws](std::string data) {
            if (onData) onData(ws, std::move(data));
        };
        if (onNewSession) onNewSession(std::move(ws));
    };
}

void NetChannel::sendData(std::string data) {
    if (session_.expired()) return;
    session_.lock()->send(std::move(data));
}

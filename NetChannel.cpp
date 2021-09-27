#include "NetChannel.h"
#include "log.h"
#include "utils.h"

NetChannel::NetChannel(asio::io_context* context)
        : context_(context), server_(*context, 3000) {
    server_.onNewSession = [this](const std::weak_ptr<tcp_session>& ws) {
        if (!session_.expired()) return;
        session_ = ws;
        auto s = session_.lock();
        LOGD("connected: %p",s.get());
        s->onClose = [this, ws]{
            LOGD("disconnected: %p", ws.lock().get());
            session_.reset();
        };
        s->onData = [this](std::string data) {
          if (onData) onData(std::move(data));
        };
    };
}

void NetChannel::sendData(std::string data) {
    if (session_.expired()) return;
    session_.lock()->send(std::move(data));
}

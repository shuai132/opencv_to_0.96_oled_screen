#include "WebsocketServer.h"
#include "log.h"
#include "utils.h"

using server_t = websocketpp::server<websocketpp::config::asio>;
using message_ptr = server_t::message_ptr;

WebsocketServer::WebsocketServer(asio::io_context* context)
        : context_(context) {
}

WebsocketServer::~WebsocketServer() {
    server_.stop_listening();
}

// todo: io_context, port, message_size
void WebsocketServer::start() {
    try {
        // Set logging settings
        server_.set_access_channels(websocketpp::log::alevel::all);
        server_.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        server_.init_asio(context_);

        server_.set_max_message_size(1024 * 1024 * 1);

        server_.set_open_handler([this](const websocketpp::connection_hdl& hdl) {
            LOGD("connected: %p", hdl.lock().get());
            hdl_ = hdl;
        });

        server_.set_close_handler([this](const websocketpp::connection_hdl& hdl) {
            LOGD("disconnected: %p", hdl.lock().get());
            hdl_ = hdl;
        });

        // Register our message handler
        server_.set_message_handler([this](const websocketpp::connection_hdl& hdl, const message_ptr& msg) {
            auto payload = msg->get_raw_payload();
            if (onData) onData((void*)payload.data(), payload.size());
        });

        // Listen on port 9002
        server_.listen(3000);

        // Start the server accept loop
        server_.start_accept();
    } catch (websocketpp::exception const& e) {
        std::cout << "websocket监听异常" << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}

void WebsocketServer::sendData(void* data, size_t len) {
    if (hdl_.lock() == nullptr) return;
    server_.send(hdl_, data, len, websocketpp::frame::opcode::BINARY);
}

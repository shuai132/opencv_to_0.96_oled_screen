#include "AsioContext.h"

AsioContext::AsioContext() {
    io_context_ptr_ = std::make_unique<asio::io_context>();
    context_ = io_context_ptr_.get();
}

void AsioContext::dispatch(const std::function<void()>& task) {
    context_->dispatch(task);
}

void AsioContext::post(const std::function<void()>& task) {
    context_->post(task);
}

void AsioContext::postDelayed(const std::function<void()>& task, uint32_t ms) {
    utils::steady_timer(context_, task, ms);
}

std::shared_ptr<asio::steady_timer>
AsioContext::timer(
        const std::function<void()>& task,
        uint32_t ms,
        const bool& loop,
        bool runFirst) {
    return utils::steady_timer(context_, task, ms, loop, runFirst);
}

asio::io_context* AsioContext::getIOContext() {
    return context_;
}

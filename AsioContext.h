#pragma once

#include <functional>
#include "noncopyable.hpp"
#include "utils.h"
#include "asio.hpp"

class AsioContext : noncopyable {
public:
    AsioContext();

    void dispatch(const std::function<void()>& task);

    void post(const std::function<void()>& task);

    void postDelayed(const std::function<void()>& task, uint32_t ms);

    std::shared_ptr<asio::steady_timer> timer(
            const std::function<void()>& task,
            uint32_t ms,
            const bool& loop = false,
            bool runFirst = false);

    asio::io_context* getIOContext();

protected:
    asio::io_context* context_;

private:
    std::unique_ptr<asio::io_context> io_context_ptr_;
};

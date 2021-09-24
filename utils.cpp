#include "utils.h"
#include <ifaddrs.h>

namespace utils {

std::shared_ptr<asio::steady_timer> steady_timer(
        asio::io_context* context,
        const std::function<void()>& cb,
        uint32_t ms,
        const bool& loop,
        bool run_first) {
    if (run_first) {
        cb();
    }

    auto timer = std::make_shared<asio::steady_timer>(*context, std::chrono::milliseconds(ms));
    timer->async_wait([timer, context, cb, ms, &loop](const asio::error_code& e) {
        if (not e) {
            cb();
        }

        if (loop) {
            steady_timer(context, cb, ms, loop);
        }
    });
    return timer;
}

std::vector<IpAddress> get_ip_address() {
  std::vector<IpAddress> ips;
  ifaddrs* pIfaddrs;
  getifaddrs(&pIfaddrs);
  auto tmp = pIfaddrs;
  while (tmp) {
    if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
      std::string ip = inet_ntoa(((struct sockaddr_in *)tmp->ifa_addr)->sin_addr);
      if (ip == "127.0.0.1") {
        tmp = tmp->ifa_next;
        continue;
      }
      ips.push_back({std::move(ip), tmp->ifa_name});
    }
    tmp = tmp->ifa_next;
  }
  freeifaddrs(pIfaddrs);
  return ips;
}

}

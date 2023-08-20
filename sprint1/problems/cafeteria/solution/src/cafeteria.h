#pragma once
#ifdef _WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include "order.h"
#include "hotdog.h"
#include "result.h"

namespace net = boost::asio;

using HotDogHandler = std::function<void(Result<HotDog> hot_dog)>;

class Cafeteria {
 public:
  explicit Cafeteria(net::io_context &io)
      : io_{io} {
  }

  void OrderHotDog(HotDogHandler handler) {
    const unsigned int order_id = ++next_order_id_;

    std::make_shared<Order>(io_, order_id, std::move(handler), *gas_cooker_, store_)->Execute();
  }

 private:
  net::io_context &io_;
  Store store_;
  std::shared_ptr<GasCooker> gas_cooker_ = std::make_shared<GasCooker>(io_);
  std::atomic_int next_order_id_ = 0;
};

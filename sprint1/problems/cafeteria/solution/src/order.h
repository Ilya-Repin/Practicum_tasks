#pragma once
#include "ingredients.h"
#include <boost/asio.hpp>
#include <memory>
#include "cafeteria.h"
#include "hotdog.h"
#include "logger.h"
#include "result.h"
#include <string_view>

using HotDogHandler = std::function<void(Result<HotDog> hot_dog)>;
/** Класс заказа
 *
 */
class Order : public std::enable_shared_from_this<Order> {
 public:
  Order(net::io_context &io, int id, HotDogHandler handler, GasCooker &gas_cooker, Store &store)
      : id_(id),
        io_(io),
        handler_(std::move(handler)),
        gas_cooker_(gas_cooker),
        bread_(store.GetBread()),
        sausage_(store.GetSausage()) {}

  void Execute() {
    logger_.LogMessage("Order has been started."sv);
    BakeBread();
    FrySausage();
  }

 private:
  int id_;
  net::io_context &io_;
  HotDogHandler handler_;
  Logger logger_{std::to_string(id_)};
  net::strand<net::io_context::executor_type> strand_{net::make_strand(io_)};
  GasCooker &gas_cooker_;

  // Ingredients
  std::shared_ptr<Sausage> sausage_;
  std::shared_ptr<Bread> bread_;

  // Flags
  bool delivered_ = false;

  // Timers
  Timer bread_timer_{io_, 1000ms};
  Timer sausage_timer_{io_, 1500ms};

  void BakeBread() {
    logger_.LogMessage("Start baking bread"sv);

    auto baking_handler = [self = shared_from_this()]() {
      self->bread_timer_.expires_from_now(Milliseconds(1000));
      self->bread_timer_.async_wait(
          net::bind_executor(self->strand_, [self = std::move(self)](sys::error_code ec) {
            self->OnBake(ec);
          }));
    };

    bread_->StartBake(gas_cooker_, baking_handler);
  }

  void FrySausage() {
    logger_.LogMessage("Start frying sausage"sv);

    auto frying_handler = [self = shared_from_this()]() {
      self->sausage_timer_.expires_from_now(Milliseconds{1500});
      self->sausage_timer_.async_wait(
          net::bind_executor(self->strand_, [self = std::move(self)](sys::error_code ec) {
            self->OnFried(ec);
          }));
    };

    sausage_->StartFry(gas_cooker_, frying_handler);
  }

  void OnBake(sys::error_code ec) {
    if (ec) {
      logger_.LogMessage("Bake error : "s + ec.what());
    } else {
      bread_->StopBaking();
      logger_.LogMessage("Bread has been baked."sv);
    }

    CheckReadiness(ec);
  }

  void OnFried(sys::error_code ec) {
    if (ec) {
      logger_.LogMessage("Fry error : "s + ec.what());
    } else {
      sausage_->StopFry();
      logger_.LogMessage("Sausage has been fried."sv);
    }

    CheckReadiness(ec);
  }

  void CheckReadiness(sys::error_code ec) {
    if (delivered_) {
      return;
    }

    if (ec) {
      delivered_ = true;
      logger_.LogMessage(ec.what());
    }

    if (bread_->IsCooked() && sausage_->IsCooked()) {
      delivered_ = true;
      handler_(HotDog{id_, sausage_, bread_});
    }
  }
};
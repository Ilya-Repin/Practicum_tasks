#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>
#include <syncstream>

#ifndef CAFETERIA_SRC_LOGGER_H_
#define CAFETERIA_SRC_LOGGER_H_

namespace net = boost::asio;
using namespace std::chrono;
using namespace std::literals;
using Timer = net::steady_timer;

class Logger {
 public:
  explicit Logger(std::string id)
      : id_(std::move(id)) {
  }

  void LogMessage(std::string_view message) const {
    std::osyncstream os{std::cout};
    os << id_ << "> ["sv << duration<double>(steady_clock::now() - start_time_).count()
       << "s] "sv << message << std::endl;
  }

 private:
  std::string id_;
  steady_clock::time_point start_time_{steady_clock::now()};
};
#endif //CAFETERIA_SRC_LOGGER_H_

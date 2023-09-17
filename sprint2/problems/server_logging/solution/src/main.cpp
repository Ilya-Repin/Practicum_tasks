#include "sdk.h"
#include <boost/asio/io_context.hpp>
#include <iostream>
#include <thread>
#include <filesystem>
#include <boost/asio/signal_set.hpp>

#include "logger.h"
#include "json_loader.h"
#include "request_handler.h"
#include "logging_request_handler.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;
namespace fs = std::filesystem;

namespace {

template<typename Fn>
void RunWorkers(unsigned int num_threads, const Fn &fn) {
  num_threads = std::max(1u, num_threads);
  std::vector<std::jthread> workers;
  workers.reserve(num_threads - 1);

  while (--num_threads) {
    workers.emplace_back(fn);
  }

  fn();
}

}  // namespace


int main(int argc, const char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: game_server <game-config-json> <path-to-static-dir>"sv << std::endl;
    return EXIT_FAILURE;
  }

  LoggerInit();

  try {
    model::Game game = json_loader::LoadGame(argv[1]);
    fs::path static_path = fs::weakly_canonical(argv[2]);
    const unsigned num_threads = std::thread::hardware_concurrency();
    net::io_context ioc(num_threads);

    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const sys::error_code &ec, [[maybe_unused]] int signal_number) {
      if (!ec) {
        ioc.stop();

        json::value custom_data{{"code"s, 0}};
        Log(custom_data, "server exited"sv);

      }
    });

    http_handler::RequestHandler handler{game, static_path};
    LoggingRequestHandler<http_handler::RequestHandler> logging_request_handler{handler};
    const auto address = net::ip::make_address("0.0.0.0");
    constexpr net::ip::port_type port = 8080;

    http_server::ServeHttp(ioc, {address, port}, [&logging_request_handler](auto&& endpoint, auto&& request, auto&& send) {
      logging_request_handler(std::forward<decltype(endpoint)>(endpoint), std::forward<decltype(request)>(request), std::forward<decltype(send)>(send));
    });

    json::value custom_data{{"port"s, port}, {"address", address.to_string()}};
    Log(custom_data, "server started"sv);

    RunWorkers(std::max(1u, num_threads), [&ioc] {
      ioc.run();
    });
  } catch (const std::exception &ex) {

    json::value custom_data{{"code"s, EXIT_FAILURE}, {"exception", ex.what()}};
    Log(custom_data, "server exited"sv);

    return EXIT_FAILURE;
  }
}
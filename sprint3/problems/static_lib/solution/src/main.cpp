#include <thread>
#include <boost/asio/signal_set.hpp>

#include "util/command_line_parser.h"
#include "json_loader/json_loader.h"
#include "http_handler/request_handler.h"
#include "logger/logging_request_handler.h"
#include "util/ticker.h"
#include "sdk.h"

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

  LoggerInit();

  try {
    if (auto args = util::ParseCommandLine(argc, argv)) {
      model::Game game = json_loader::LoadGame(args.value().config_file);
      app::Application app{game, args.value().randomize_spawn_point};

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

      auto api_strand = net::make_strand(ioc);
      std::shared_ptr<http_handler::RequestHandler>
          handler = std::make_shared<http_handler::RequestHandler>(app, api_strand, args.value());

      if (args.value().tick_period) {
        auto ticker = std::make_shared<util::Ticker>(api_strand, 50ms,
                                               [&app](std::chrono::milliseconds delta) { app.Tick(delta.count()); }
        );

        ticker->Start();
      }

      LoggingRequestHandler<http_handler::RequestHandler> logging_request_handler(*handler);
      const auto address = net::ip::make_address("0.0.0.0");
      constexpr net::ip::port_type port = 8080;

      http_server::ServeHttp(ioc,
                             {address, port},
                             [&logging_request_handler](auto &&endpoint, auto &&request, auto &&send) {
                               logging_request_handler(std::forward<decltype(endpoint)>(endpoint),
                                                       std::forward<decltype(request)>(request),
                                                       std::forward<decltype(send)>(send));
                             });

      json::value custom_data{{"port"s, port}, {"address", address.to_string()}};
      Log(custom_data, "server started"sv);

      RunWorkers(std::max(1u, num_threads), [&ioc] {
        ioc.run();
      });
    }
  } catch (const std::exception &ex) {

    json::value custom_data{{"code"s, EXIT_FAILURE}, {"exception", ex.what()}};
    Log(custom_data, "server exited"sv);

    return EXIT_FAILURE;
  }
}

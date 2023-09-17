#pragma once

#include <filesystem>
#include <string_view>
#include <boost/system/error_code.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "logger.h"
#include "../constants_storage.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

template<class SomeRequestHandler>
class LoggingRequestHandler {
  template<typename Body, typename Allocator>
  static void LogRequest(const net::ip::tcp::endpoint &endpoint,
                         const http::request<Body, http::basic_fields<Allocator>> &r) {
    json::value custom_data{{"ip", endpoint.address().to_string()},
                            {"URI", r.target()},
                            {"method", r.method_string()}};

    Log(custom_data, "request received"sv);
  }

  template<typename T>
  static void LogResponse(const net::ip::tcp::endpoint &endpoint,
                          const http::response<T> &response,
                          const std::chrono::system_clock::duration &duration) {

    std::string_view content_type = response[http::field::content_type];

    if (content_type.empty()) {
      content_type = "null"sv;
    }

    json::value custom_data{{"ip", endpoint.address().to_string()},
                            {"response_time", std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()},
                            {"code", response.result_int()},
                            {"content_type", content_type}};

    Log(custom_data, "response sent"sv);
  }

 public:
  template<typename Body, typename Allocator, typename Send>
  void operator()(net::ip::tcp::endpoint &&endpoint,
                  http::request<Body, http::basic_fields<Allocator>> &&req,
                  Send &&send) {
    LogRequest(endpoint, req);

    auto log_send =
        [endpoint, start_time = std::chrono::system_clock::now(), send = std::forward<Send>(send)](auto &&response) {
          auto end_time = std::chrono::system_clock::now();
          LogResponse(endpoint, response, end_time - start_time);
          send(std::forward<decltype(response)>(response));
        };

    decorated_(std::forward<decltype(req)>(req), log_send);
  }

  LoggingRequestHandler(SomeRequestHandler &h) : decorated_(h) {}

 private:
  SomeRequestHandler &decorated_;
};
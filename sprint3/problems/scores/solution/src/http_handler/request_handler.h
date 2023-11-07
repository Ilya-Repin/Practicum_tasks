#pragma once

#include <filesystem>

#include "api_handler.h"

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace fs = std::filesystem;
namespace sys = boost::system;
namespace net = boost::asio;

using namespace std::literals;
using StringResponse = http::response<http::string_body>;
using FileResponse = http::response<http::file_body>;

StringResponse MakeStringResponse(http::status status, std::string_view body, size_t http_version,
                                  bool keep_alive,
                                  std::string_view content_type = ContentType::APPLICATION_JSON);

class RequestHandler : public std::enable_shared_from_this<RequestHandler> {
 public:
  using Strand = net::strand<net::io_context::executor_type>;

  explicit RequestHandler(app::Application &app, Strand &strand, util::Args &args)
      : api_strand_(strand), static_path_(fs::weakly_canonical(args.www_root)) {
    api_handler_ = std::make_unique<ApiHandler>(app, args);
  }

  RequestHandler(const RequestHandler &) = delete;
  RequestHandler &operator=(const RequestHandler &) = delete;

  template<typename Body, typename Allocator, typename Send>
  void operator()(http::request<Body, http::basic_fields<Allocator>> &&req, Send &&send) {

    if (req.target().starts_with(SystemPaths::API)) {
      auto handle = [self = shared_from_this(), send,
          req = std::forward<decltype(req)>(req)] {
        auto response = self->api_handler_->HandleApiRequest(req);
        send(response);
      };

      net::dispatch(api_strand_, handle);

    } else if (!req.target().starts_with(SystemPaths::API)) {
      if (req.method() != http::verb::get && req.method() != http::verb::head) {
        send(MakeStringResponse(http::status::method_not_allowed,
                                HttpResponseConstants::MESSAGE_INVALID_METHOD,
                                req.version(),
                                req.keep_alive(),
                                ContentType::TEXT_HTML));

        return;
      }

      try {
        HandleStaticFileRequest(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
      } catch (std::exception &e) {
        boost::json::object error_obj;

        error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV(HttpResponseConstants::CODE_400);
        error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV(HttpResponseConstants::MESSAGE_400);

        send(MakeStringResponse(http::status::bad_request,
                                std::string_view(boost::json::serialize(error_obj)),
                                req.version(),
                                req.keep_alive()));
      }
    }
  }

 private:
  Strand &api_strand_;
  std::unique_ptr<ApiHandler> api_handler_;
  fs::path static_path_;

  // Methods
  std::string DecodeURL(std::string_view target);

  fs::path GetPath(std::string_view url);

  bool CheckFileInRoot(fs::path &path);

  bool CheckFileExistence(fs::path &path);

  template<typename Body, typename Allocator, typename Send>
  void HandleStaticFileRequest(http::request<Body, http::basic_fields<Allocator>> &&req, Send &&send) {
    fs::path path = GetPath(req.target());

    if (!CheckFileInRoot(path)) {
      send(MakeStringResponse(http::status::bad_request,
                              HttpResponseConstants::MESSAGE_400,
                              req.version(),
                              req.keep_alive(),
                              ContentType::TEXT_PLAIN));

      return;
    }

    if (fs::is_directory(path)) {
      if (!path.string().ends_with('/')) {
        path += "/";
      }

      path += "index.html";
    }

    if (!CheckFileExistence(path)) {
      send(MakeStringResponse(http::status::not_found,
                              HttpResponseConstants::MESSAGE_404,
                              req.version(),
                              req.keep_alive(),
                              ContentType::TEXT_PLAIN));

      return;
    }

    http::file_body::value_type file;

    if (sys::error_code ec; file.open(path.string().data(), beast::file_mode::read, ec), ec) {
      throw std::logic_error("Failed to open file "s + path.string() + "\n"s);
    }

    FileResponse res;
    res.version(11);  // HTTP/1.1
    res.result(http::status::ok);

    if (ContentType::extensions.contains(fs::path(path).extension().string())) {
      res.insert(http::field::content_type, ContentType::extensions.at(fs::path(path).extension().string()));
    }

    if (req.method() == http::verb::get) {
      res.body() = std::move(file);
    }

    res.prepare_payload();
    send(res);
  }
};

}  // namespace http_handler

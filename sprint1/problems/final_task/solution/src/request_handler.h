#pragma once

#include <boost/json.hpp>
#include "http_server.h"
#include "model.h"

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;

using namespace std::literals;
using StringResponse = http::response<http::string_body>;

struct ContentType {
  ContentType() = delete;
  constexpr static std::string_view
  APPLICATION_JSON = "application/json"sv;
  constexpr static std::string_view
  TEXT_HTML = "text/html"sv;
};

StringResponse MakeStringResponse(http::status status, std::string_view body, size_t http_version,
                                  bool keep_alive,
                                  std::string_view content_type = ContentType::APPLICATION_JSON);

class RequestHandler {
 public:
  explicit RequestHandler(model::Game &game) : game_{game} {
  }

  RequestHandler(const RequestHandler &) = delete;
  RequestHandler &operator=(const RequestHandler &) = delete;

  template<typename Body, typename Allocator, typename Send>
  void operator()(http::request<Body, http::basic_fields<Allocator>> &&req, Send &&send) {
    const auto text_response = [&req, &send](http::status status, std::string_view text) {
      send(MakeStringResponse(status, text, req.version(), req.keep_alive()));
    };

    if (req.method() != http::verb::get && req.method() != http::verb::head) {
      send(MakeStringResponse(http::status::method_not_allowed,
                              "Invalid method"sv,
                              req.version(),
                              req.keep_alive(),
                              ContentType::TEXT_HTML));
      return;
    }

    if (req.target() == "/api/v1/maps") {
      std::shared_ptr<boost::json::array> response_arr = MakeMapsArr();

      text_response(http::status::ok, std::string_view(boost::json::serialize(*response_arr)));
    } else if (req.target().starts_with("/api/v1/maps/")) {
      std::string map_id = std::string(req.target()).substr(strlen("/api/v1/maps/"));
      auto map = game_.FindMap(model::Map::Id(map_id));

      if (map) {
        boost::json::object map_obj;
        ConfigureJsonMap(map_obj, map);
        text_response(http::status::ok, std::string_view(boost::json::serialize(map_obj)));
      } else {
        boost::json::object error_obj;

        error_obj["code"] = "mapNotFound";
        error_obj["message"] = "Map not found";

        text_response(http::status::not_found, std::string_view(boost::json::serialize(error_obj)));
      }
    } else {
      boost::json::object error_obj;

      error_obj["code"] = "badRequest";
      error_obj["message"] = "Bad request";

      text_response(http::status::bad_request, std::string_view(boost::json::serialize(error_obj)));
    }
  }

 private:
  std::shared_ptr<boost::json::array> MakeMapsArr();

  std::shared_ptr<boost::json::array> MakeRoadsArr(const model::Map *map);

  std::shared_ptr<boost::json::array> MakeBuildingsArr(const model::Map *map);

  std::shared_ptr<boost::json::array> MakeOfficesArr(const model::Map *map);

  void ConfigureJsonMap(boost::json::object &map_obj, const model::Map *map);

  model::Game &game_;
};

}  // namespace http_handler

#pragma once

#include <variant>
#include <boost/json.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>

#include "../http_server/http_server.h"
#include "../constants_storage.h"
#include "../app/Application.h"
#include "../util/command_line_parser.h"

namespace http_handler {

namespace sys = boost::system;
namespace beast = boost::beast;
namespace http = beast::http;

using namespace constants;
using namespace std::literals;

using JsonResponse = http::response<http::string_body>;
using StringResponse = http::response<http::string_body>;

JsonResponse MakeJsonResponse(http::status status, std::string_view body, size_t http_version,
                              bool keep_alive,
                              std::string_view content_type = ContentType::APPLICATION_JSON);

JsonResponse MakeWrongMethodError(std::string_view message, std::string_view allowed_method, size_t http_version);
JsonResponse MakeBadRequestError(std::string_view message, size_t http_version);
JsonResponse MakeMapNotFoundError(std::string_view message, size_t http_version);

class ApiHandler {
 public:
  ApiHandler(app::Application &app, util::Args &args) : app_(app), manual_tick(!args.tick_period) {
  }

  ApiHandler(const ApiHandler &) = delete;
  ApiHandler &operator=(const ApiHandler &) = delete;

  StringResponse HandleApiRequest(const http::request<http::string_body> &req);

 private:
  app::Application &app_;
  bool manual_tick;

  // Methods
  void ConfigureJsonMap(boost::json::object &map_obj, const std::shared_ptr<model::Map> map);

  std::shared_ptr<boost::json::array> MakeMapsArr();

  std::shared_ptr<boost::json::array> MakeRoadsArr(const std::shared_ptr<model::Map> map);

  std::shared_ptr<boost::json::array> MakeBuildingsArr(const std::shared_ptr<model::Map> map);

  std::shared_ptr<boost::json::array> MakeOfficesArr(const std::shared_ptr<model::Map> map);

  void ValidateUserName(std::string &body);

  std::variant<JsonResponse, std::shared_ptr<app::Token>> GetToken(const http::request<http::string_body> &req);

  JsonResponse GetMaps(const http::request<http::string_body> &req);

  JsonResponse GetMap(const http::request<http::string_body> &req);

  JsonResponse JoinGame(const http::request<http::string_body> &req);

  StringResponse GetPlayers(const http::request<http::string_body> &req);

  StringResponse GetGameState(const http::request<http::string_body> &req);

  StringResponse DoAction(const http::request<http::string_body> &req);

  StringResponse HandleTickRequest(const http::request<http::string_body> &req);
};

} // http_handler
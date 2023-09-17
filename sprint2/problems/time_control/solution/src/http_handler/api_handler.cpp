#include <variant>
#include "api_handler.h"
#include <iomanip>

namespace sys = boost::system;
namespace http_handler {
std::shared_ptr<boost::json::array> ApiHandler::MakeMapsArr() {
  boost::json::array maps_arr;
  auto maps = app_.ListMaps();

  for (auto map : maps) {
    boost::json::object map_obj;
    map_obj[MakeBoostSV(JsonAttributes::ID)] = *map->GetId();
    map_obj[MakeBoostSV(JsonAttributes::NAME)] = map->GetName();
    maps_arr.push_back(std::move(map_obj));
  }

  return std::make_shared<boost::json::array>(maps_arr);
}

std::shared_ptr<boost::json::array> ApiHandler::MakeRoadsArr(const std::shared_ptr<model::Map> map) {
  boost::json::array roads_arr;
  auto roads = map->GetRoads();

  for (auto road : roads) {
    boost::json::object road_obj;

    if (road.IsHorizontal()) {
      road_obj[MakeBoostSV(JsonAttributes::X0)] = road.GetStart().x;
      road_obj[MakeBoostSV(JsonAttributes::Y0)] = road.GetStart().y;
      road_obj[MakeBoostSV(JsonAttributes::X1)] = road.GetEnd().x;
    } else {
      road_obj[MakeBoostSV(JsonAttributes::X0)] = road.GetStart().x;
      road_obj[MakeBoostSV(JsonAttributes::Y0)] = road.GetStart().y;
      road_obj[MakeBoostSV(JsonAttributes::Y1)] = road.GetEnd().y;
    }

    roads_arr.push_back(std::move(road_obj));
  }

  return std::make_shared<boost::json::array>(roads_arr);
}

std::shared_ptr<boost::json::array> ApiHandler::MakeBuildingsArr(const std::shared_ptr<model::Map> map) {
  boost::json::array buildings_arr;
  auto buildings = map->GetBuildings();

  for (auto building : buildings) {
    boost::json::object building_obj;

    building_obj[MakeBoostSV(JsonAttributes::X)] = building.GetBounds().position.x;
    building_obj[MakeBoostSV(JsonAttributes::Y)] = building.GetBounds().position.y;
    building_obj[MakeBoostSV(JsonAttributes::WIDTH)] = building.GetBounds().size.width;
    building_obj[MakeBoostSV(JsonAttributes::HEIGHT)] = building.GetBounds().size.height;

    buildings_arr.push_back(std::move(building_obj));
  }

  return std::make_shared<boost::json::array>(buildings_arr);
}

std::shared_ptr<boost::json::array> ApiHandler::MakeOfficesArr(const std::shared_ptr<model::Map> map) {
  boost::json::array offices_arr;
  auto offices = map->GetOffices();

  for (auto office : offices) {
    boost::json::object office_obj;

    office_obj[MakeBoostSV(JsonAttributes::ID)] = *office.GetId();
    office_obj[MakeBoostSV(JsonAttributes::X)] = office.GetPosition().x;
    office_obj[MakeBoostSV(JsonAttributes::Y)] = office.GetPosition().y;
    office_obj[MakeBoostSV(JsonAttributes::OFFSET_X)] = office.GetOffset().dx;
    office_obj[MakeBoostSV(JsonAttributes::OFFSET_Y)] = office.GetOffset().dy;

    offices_arr.push_back(std::move(office_obj));
  }

  return std::make_shared<boost::json::array>(offices_arr);
}

void ApiHandler::ConfigureJsonMap(boost::json::object &map_obj, const std::shared_ptr<model::Map> map) {
  map_obj[MakeBoostSV(JsonAttributes::ID)] = *map->GetId();
  map_obj[MakeBoostSV(JsonAttributes::NAME)] = map->GetName();

  std::shared_ptr<boost::json::array> roads = MakeRoadsArr(map);
  map_obj[MakeBoostSV(JsonAttributes::ROADS)] = *roads;

  std::shared_ptr<boost::json::array> buildings = MakeBuildingsArr(map);
  map_obj[MakeBoostSV(JsonAttributes::BUILDINGS)] = *buildings;

  std::shared_ptr<boost::json::array> offices = MakeOfficesArr(map);
  map_obj[MakeBoostSV(JsonAttributes::OFFICES)] = *offices;
}

JsonResponse MakeJsonResponse(http::status status, std::string_view body, size_t http_version,
                              bool keep_alive,
                              std::string_view content_type) {
  JsonResponse response(status, http_version);
  response.set(http::field::content_type, content_type);
  response.body() = body;
  response.content_length(body.size());
  response.keep_alive(keep_alive);
  response.set(http::field::cache_control, "no-cache");
  return response;
}

JsonResponse MakeWrongMethodError(std::string_view message, std::string_view allowed_method, size_t http_version) {
  boost::json::object error_obj;
  error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV("invalidMethod");
  error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV(message);
  JsonResponse response(http::status::method_not_allowed, http_version);
  response.set(http::field::content_type, ContentType::APPLICATION_JSON);
  response.body() = boost::json::serialize(error_obj);
  response.set(http::field::allow, allowed_method);
  response.content_length(response.body().size());
  response.set(http::field::cache_control, "no-cache");

  return response;
}

JsonResponse MakeBadRequestError(std::string_view message, size_t http_version) {
  boost::json::object error_obj;
  error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV("invalidArgument");
  error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV(message);
  JsonResponse response(http::status::bad_request, http_version);
  response.set(http::field::content_type, ContentType::APPLICATION_JSON);
  response.body() = boost::json::serialize(error_obj);
  response.content_length(response.body().size());
  response.set(http::field::cache_control, "no-cache");

  return response;
}

JsonResponse MakeUnauthorizedError(std::string_view code, std::string_view message, size_t http_version) {
  boost::json::object error_obj;
  error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV(code);
  error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV(message);
  JsonResponse response(http::status::unauthorized, http_version);
  response.set(http::field::content_type, ContentType::APPLICATION_JSON);
  response.body() = boost::json::serialize(error_obj);
  response.content_length(response.body().size());
  response.set(http::field::cache_control, "no-cache");

  return response;
}

JsonResponse MakeMapNotFoundError(std::string_view message, size_t http_version) {
  boost::json::object error_obj;
  error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV("mapNotFound");
  error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV(message);
  JsonResponse response(http::status::not_found, http_version);
  response.set(http::field::content_type, ContentType::APPLICATION_JSON);
  response.body() = boost::json::serialize(error_obj);
  response.content_length(response.body().size());
  response.set(http::field::cache_control, "no-cache");

  return response;
}

JsonResponse MakeInternalError(std::string_view message, size_t http_version) {
  StringResponse response(http::status::internal_server_error, http_version);
  response.set(http::field::content_type, ContentType::TEXT_HTML);
  response.body() = message;
  response.content_length(response.body().size());
  response.set(http::field::cache_control, "no-cache");

  return response;
}

StringResponse ApiHandler::GetMaps(const http::request<http::string_body> &req) {
  if (req.method() != http::verb::get && req.method() != http::verb::head) {
    return MakeWrongMethodError("Only GET, HEAD methods are expected", "GET, HEAD", req.version());
  } else {
    std::shared_ptr<boost::json::array> response_arr = MakeMapsArr();
    return MakeJsonResponse(http::status::ok,
                            std::string_view(boost::json::serialize(*response_arr)),
                            req.version(),
                            req.keep_alive());
  }
}

StringResponse ApiHandler::GetMap(const http::request<http::string_body> &req) {
  if (req.method() != http::verb::get && req.method() != http::verb::head) {
    return MakeWrongMethodError("Only GET, HEAD methods are expected", "GET, HEAD", req.version());
  } else {
    size_t len_path = SystemPaths::MAPS.size() + "/"sv.size();
    std::string map_id = std::string(req.target()).substr(len_path);
    auto map = app_.GetMap(model::Map::Id(map_id));

    if (map) {

      boost::json::object map_obj;
      ConfigureJsonMap(map_obj, map);
      return MakeJsonResponse(http::status::ok,
                              std::string_view(boost::json::serialize(map_obj)),
                              req.version(),
                              req.keep_alive());
    } else {
      boost::json::object error_obj;

      error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV(HttpStatus::CODE_MAP_404);
      error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV(HttpStatus::MESSAGE_MAP_404);

      return MakeJsonResponse(http::status::not_found,
                              std::string_view(boost::json::serialize(error_obj)),
                              req.version(),
                              req.keep_alive());
    }
  }
}

StringResponse ApiHandler::JoinGame(const http::request<http::string_body> &req) {
  if (req.method() != http::verb::post) {
    return MakeWrongMethodError("Only POST methods are expected", "POST", req.version());
  }

  json::value name_json, map_json;

  try {
    std::string body = req.body();
    std::string prefix = ":";

    if (body[body.find(prefix) + 1] == ' ') {
      body.replace(body.find(prefix) + 1, 1, "");
    }

    size_t startpos = body.find(prefix) + prefix.length() + 1;
    size_t endpos = body.rfind('"', body.rfind(',', body.find("mapId", startpos)));
    std::string raw_value = body.substr(startpos, endpos - startpos);
    // форматирование значения поля userName
    std::string cleaned_value = "";
    for (int i = 0; i < raw_value.size(); ++i) {
      char c = raw_value[i];
      if (isalnum(c)) {
        cleaned_value += c;
      }
    }
    body = body.substr(0, startpos) + cleaned_value + body.substr(endpos);
    auto value = json::parse(body);
    name_json = value.at("userName");
    map_json = value.at("mapId");

  } catch (std::exception &e) {
    return MakeBadRequestError("Join game request parse error", req.version());
  }

  if (std::string(name_json.as_string()).empty()) {
    return MakeBadRequestError("Invalid name", req.version());
  }
  if (std::string(map_json.as_string()).empty()) {
    return MakeBadRequestError("Invalid mapId", req.version());
  }

  model::Map::Id map_id{std::string(map_json.as_string())};
  std::string dog_name(name_json.as_string());

  try {

    if (app_.GetMap(map_id)) {

      std::pair<app::Token, model::Dog::Id> dog_data = app_.JoinGame(dog_name, map_id);

      boost::json::object res_obj;

      res_obj[MakeBoostSV("authToken")] = MakeBoostSV(*dog_data.first);
      res_obj[MakeBoostSV("playerId")] = *dog_data.second;

      return MakeJsonResponse(http::status::ok,
                              std::string_view(boost::json::serialize(res_obj)),
                              req.version(),
                              req.keep_alive());

    }

    return MakeMapNotFoundError("Map not found", req.version());

  }
  catch (const std::exception &e) {
    return MakeInternalError("Error joining game", req.version());
  }

}

std::variant<JsonResponse,
             std::shared_ptr<app::Token>> ApiHandler::GetToken(const http::request<http::string_body> &req) {
  if (req.find("authorization") == req.end()) {
    return MakeUnauthorizedError("invalidToken", "Authorization header is missing", req.version());
  }

  std::string auth = std::string(req.at("Authorization"));
  size_t pos = auth.find("Bearer "); // ищем позицию подстроки "Bearer "
  if (pos != std::string::npos) { // если подстрока найдена
    if (auth.substr(pos + "Bearer "sv.length()).size() != 32) {
      return MakeUnauthorizedError("invalidToken", "Invalid token", req.version());
    }
    app::Token token(*new app::Token(auth.substr(pos + "Bearer "sv.length())));

    if (!app_.IsTokenExist(token)) {
      return MakeUnauthorizedError("unknownToken", "Player token has not been found", req.version());
    }

    return std::make_shared<app::Token>(token);
  }

  return MakeUnauthorizedError("invalidToken", "Authorization header is missing", req.version());
}

StringResponse ApiHandler::GetPlayers(const http::request<http::string_body> &req) {
  if (req.method() != http::verb::get && req.method() != http::verb::head) {
    return MakeWrongMethodError("Only GET, HEAD methods ar9e expected", "GET, HEAD", req.version());
  }

  std::variant<JsonResponse, std::shared_ptr<app::Token>> result = GetToken(req);
  if (result.index() == 1) {
    std::shared_ptr<app::Token> token = std::get<std::shared_ptr<app::Token>>(result);
    auto dogs = app_.ListPlayers(*token);
    boost::json::object res_obj;
    for (auto &dog : dogs) {
      boost::json::object player;
      player["name"] = dog.second;
      res_obj[std::to_string(dog.first)] = player;
    }

    return MakeJsonResponse(http::status::ok,
                            std::string_view(boost::json::serialize(res_obj)),
                            req.version(),
                            req.keep_alive());
  }

   JsonResponse response = std::get<JsonResponse>(result);
   return response;


}

StringResponse ApiHandler::GetGameState(const http::request<http::string_body> &req) {
  if (req.method() != http::verb::get && req.method() != http::verb::head) {
    return MakeWrongMethodError("Only GET, HEAD methods are expected", "GET, HEAD", req.version());
  }
  std::variant<JsonResponse, std::shared_ptr<app::Token>> result = GetToken(req);
  if (result.index() == 1) {
    std::shared_ptr<app::Token> token = std::get<std::shared_ptr<app::Token>>(result);

    auto game_state = app_.GetGameState(*token);
    boost::json::object players;

    for (auto data = game_state.rbegin(); data != game_state.rend(); ++data) {
      boost::json::object params;

      boost::json::array pos_array{data->pos.x, data->pos.y};
      params["pos"] = pos_array;

      boost::json::array speed_array{data->speed.horizontal, data->speed.vertical};
      params["speed"] = speed_array;

      params["dir"] = data->dir;

      players[std::to_string(*data->id)] = params;
    }

    boost::json::object res_object;
    res_object["players"] = players;
    return MakeJsonResponse(http::status::ok,
                            std::string_view(boost::json::serialize(res_object)),
                            req.version(),
                            req.keep_alive());
  }

  JsonResponse response = std::get<JsonResponse>(result);
  return response;
}

StringResponse ApiHandler::DoAction(const http::request<http::string_body> &req) {
  if (req.method() != http::verb::post) {
    return MakeWrongMethodError("Only POST methods are expected", "POST", req.version());
  }
  if (req.count(http::field::content_type) == 0 || req.at(http::field::content_type) != ContentType::APPLICATION_JSON) {
    return MakeBadRequestError("Invalid content type", req.version());
  }

  std::variant<JsonResponse, std::shared_ptr<app::Token>> result = GetToken(req);
  if (result.index() == 1) {
    std::shared_ptr<app::Token> token = std::get<std::shared_ptr<app::Token>>(result);

    json::value value, move;
    try {
      value = json::parse(req.body());
      move = value.at("move");
    } catch (std::exception &e) {
      return MakeBadRequestError("Failed to parse action", req.version());
    }

    std::string dir(move.as_string());

    if (dir.size() > 1 || (dir.size() == 1 && std::string("LRUD").find(dir) == std::string::npos)) {
      return MakeBadRequestError("Failed to parse action", req.version());
    }

    app_.DoAction(*token, dir);

    return MakeJsonResponse(http::status::ok,
                            std::string_view(boost::json::serialize(boost::json::object())),
                            req.version(),
                            req.keep_alive());
  }
    
 JsonResponse response = std::get<JsonResponse>(result);
 return response;

}

StringResponse ApiHandler::HandleTickRequest(const http::request<http::string_body> &req) {
  if (req.method() != http::verb::post) {
    return MakeWrongMethodError("Only POST methods are expected", "POST", req.version());
  }
  if (req.count(http::field::content_type) == 0
      || req.at(http::field::content_type) != ContentType::APPLICATION_JSON) {
    return MakeBadRequestError("Invalid content type", req.version());
  }

  uint64_t time_delta;

  try {
    auto value = json::parse(req.body());
    time_delta = value.at("timeDelta").as_int64();
  } catch (std::exception &e) {
    return MakeBadRequestError("Failed to parse tick request JSON", req.version());
  }

  app_.Tick(static_cast<uint64_t>(time_delta));

  return MakeJsonResponse(http::status::ok,
                          std::string_view(boost::json::serialize(boost::json::object())),
                          req.version(),
                          req.keep_alive());

}

StringResponse ApiHandler::HandleApiRequest(const http::request<http::string_body> &req) {
  StringResponse res;

  std::string_view target = req.target();
  if (target == SystemPaths::MAPS) {
    return GetMaps(req);
  } else if (target.starts_with(SystemPaths::MAPS)) {
    return GetMap(req);
  } else if (target == SystemPaths::JOIN) {
    return JoinGame(req);
  } else if (target == SystemPaths::PLAYERS) {
    return GetPlayers(req);
  } else if (target == SystemPaths::GAME_STATE) {
    return GetGameState(req);
  } else if (target == SystemPaths::ACTION) {
    return DoAction(req);
  } else if (target == SystemPaths::TICK) {
    return HandleTickRequest(req);
  }

  boost::json::object error_obj;
  error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV("badRequest");
  error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV("Bad Request");
  return MakeJsonResponse(http::status::bad_request,
                          boost::json::serialize(error_obj),
                          req.version(),
                          req.keep_alive());

}

}
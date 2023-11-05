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

  map_obj[MakeBoostSV(JsonAttributes::LOOT_TYPES)] = map->GetExtraData().GetJsonArray();
}

void ApiHandler::ValidateUserName(std::string &body) {
  std::string_view prefix = ":"sv;

  if (body[body.find(prefix) + 1] == ' ') {
    body.replace(body.find(prefix) + 1, 1, "");
  }

  size_t start_pos = body.find(prefix) + prefix.length() + 1;
  size_t end_pos = body.rfind('"', body.rfind(',', body.find("mapId", start_pos)));
  std::string raw_value = body.substr(start_pos, end_pos - start_pos);

  std::string cleaned_value = "";

  for (int i = 0; i < raw_value.size(); ++i) {
    char c = raw_value[i];

    if (isalnum(c)) {
      cleaned_value += c;
    }
  }

  body = body.substr(0, start_pos) + cleaned_value + body.substr(end_pos);
}

JsonResponse MakeJsonResponse(http::status status, std::string_view body, size_t http_version,
                              bool keep_alive,
                              std::string_view content_type) {
  JsonResponse response(status, http_version);
  response.set(http::field::content_type, content_type);
  response.body() = body;
  response.content_length(body.size());
  response.keep_alive(keep_alive);
  response.set(http::field::cache_control, HttpResponseConstants::NO_CACHE);

  return response;
}

JsonResponse MakeWrongMethodError(std::string_view message, std::string_view allowed_method, size_t http_version) {
  boost::json::object error_obj;

  error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV(HttpResponseConstants::CODE_INVALID_METHOD);
  error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV(message);

  JsonResponse response(http::status::method_not_allowed, http_version);

  response.set(http::field::content_type, ContentType::APPLICATION_JSON);
  response.body() = boost::json::serialize(error_obj);
  response.set(http::field::allow, allowed_method);
  response.content_length(response.body().size());
  response.set(http::field::cache_control, HttpResponseConstants::NO_CACHE);

  return response;
}

JsonResponse MakeBadRequestError(std::string_view message, size_t http_version) {
  boost::json::object error_obj;

  error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV(HttpResponseConstants::CODE_INVALID_ARGUMENT);
  error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV(message);

  JsonResponse response(http::status::bad_request, http_version);

  response.set(http::field::content_type, ContentType::APPLICATION_JSON);
  response.body() = boost::json::serialize(error_obj);
  response.content_length(response.body().size());
  response.set(http::field::cache_control, HttpResponseConstants::NO_CACHE);

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
  response.set(http::field::cache_control, HttpResponseConstants::NO_CACHE);

  return response;
}

JsonResponse MakeMapNotFoundError(std::string_view message, size_t http_version) {
  boost::json::object error_obj;

  error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV(HttpResponseConstants::CODE_MAP_NOT_FOUND);
  error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV(message);

  JsonResponse response(http::status::not_found, http_version);

  response.set(http::field::content_type, ContentType::APPLICATION_JSON);
  response.body() = boost::json::serialize(error_obj);
  response.content_length(response.body().size());
  response.set(http::field::cache_control, HttpResponseConstants::NO_CACHE);

  return response;
}

JsonResponse MakeInternalError(std::string_view message, size_t http_version) {
  StringResponse response(http::status::internal_server_error, http_version);

  response.set(http::field::content_type, ContentType::TEXT_HTML);
  response.body() = message;
  response.content_length(response.body().size());
  response.set(http::field::cache_control, HttpResponseConstants::NO_CACHE);

  return response;
}

StringResponse ApiHandler::GetMaps(const http::request<http::string_body> &req) {
  if (req.method() != http::verb::get && req.method() != http::verb::head) {
    return MakeWrongMethodError(HttpResponseConstants::GET_HEAD_EXPECTED,
                                HttpResponseConstants::ALLOWED_GET_HEAD,
                                req.version());
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
    return MakeWrongMethodError(HttpResponseConstants::GET_HEAD_EXPECTED,
                                HttpResponseConstants::ALLOWED_GET_HEAD,
                                req.version());
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

      error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV(HttpResponseConstants::CODE_MAP_404);
      error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV(HttpResponseConstants::MESSAGE_MAP_404);

      return MakeJsonResponse(http::status::not_found,
                              std::string_view(boost::json::serialize(error_obj)),
                              req.version(),
                              req.keep_alive());
    }
  }
}

StringResponse ApiHandler::JoinGame(const http::request<http::string_body> &req) {
  if (req.method() != http::verb::post) {
    return MakeWrongMethodError(HttpResponseConstants::POST_EXPECTED,
                                HttpResponseConstants::ALLOWED_POST,
                                req.version());
  }

  json::value name_json, map_json;

  try {
    std::string body = req.body();
    ValidateUserName(body);

    auto value = json::parse(body);

    name_json = value.at(MakeBoostSV(JsonAttributes::USERNAME));
    map_json = value.at(MakeBoostSV(JsonAttributes::MAP_ID));

  } catch (std::exception &e) {
    return MakeBadRequestError(HttpResponseConstants::JOIN_GAME_PARSE_ERROR, req.version());
  }

  if (std::string(name_json.as_string()).empty()) {
    return MakeBadRequestError(HttpResponseConstants::MESSAGE_INVALID_NAME, req.version());
  }

  if (std::string(map_json.as_string()).empty()) {
    return MakeBadRequestError(HttpResponseConstants::MESSAGE_INVALID_MAP_ID, req.version());
  }

  model::Map::Id map_id{std::string(map_json.as_string())};
  std::string dog_name(name_json.as_string());

  try {
    if (app_.GetMap(map_id)) {

      std::pair<app::Token, model::Dog::Id> dog_data = app_.JoinGame(dog_name, map_id);

      boost::json::object res_obj;

      res_obj[MakeBoostSV(JsonAttributes::TOKEN)] = MakeBoostSV(*dog_data.first);
      res_obj[MakeBoostSV(JsonAttributes::PLAYER_ID)] = *dog_data.second;

      return MakeJsonResponse(http::status::ok,
                              std::string_view(boost::json::serialize(res_obj)),
                              req.version(),
                              req.keep_alive());
    }

    return MakeMapNotFoundError(HttpResponseConstants::MESSAGE_MAP_404, req.version());
  }
  catch (const std::exception &e) {
    return MakeInternalError(HttpResponseConstants::JOIN_GAME_ERROR, req.version());
  }
}

std::variant<JsonResponse,
             std::shared_ptr<app::Token>> ApiHandler::GetToken(const http::request<http::string_body> &req) {
  if (req.find("authorization") == req.end()) {
    return MakeUnauthorizedError(HttpResponseConstants::CODE_INVALID_TOKEN,
                                 HttpResponseConstants::MESSAGE_AUTH_HEADER_MISSING,
                                 req.version());
  }

  std::string auth = std::string(req.at("Authorization"));
  size_t pos = auth.find("Bearer ");

  if (pos != std::string::npos) {

    if (auth.substr(pos + "Bearer "sv.length()).size() != 32) {
      return MakeUnauthorizedError(HttpResponseConstants::CODE_INVALID_TOKEN,
                                   HttpResponseConstants::MESSAGE_INVALID_TOKEN,
                                   req.version());
    }

    app::Token token(*new app::Token(auth.substr(pos + "Bearer "sv.length())));

    if (!app_.IsTokenExist(token)) {
      return MakeUnauthorizedError(HttpResponseConstants::CODE_UNKNOWN_TOKEN,
                                   HttpResponseConstants::MESSAGE_TOKEN_NOT_FOUND,
                                   req.version());
    }

    return std::make_shared<app::Token>(token);
  }

  return MakeUnauthorizedError(HttpResponseConstants::CODE_INVALID_TOKEN,
                               HttpResponseConstants::MESSAGE_AUTH_HEADER_MISSING,
                               req.version());
}

StringResponse ApiHandler::GetPlayers(const http::request<http::string_body> &req) {
  if (req.method() != http::verb::get && req.method() != http::verb::head) {
    return MakeWrongMethodError(HttpResponseConstants::GET_HEAD_EXPECTED,
                                HttpResponseConstants::ALLOWED_GET_HEAD,
                                req.version());
  }

  std::variant<JsonResponse, std::shared_ptr<app::Token>> result = GetToken(req);

  if (result.index() == 1) {
    std::shared_ptr<app::Token> token = std::get<std::shared_ptr<app::Token>>(result);

    auto dogs = app_.ListPlayers(*token);
    boost::json::object res_obj;

    for (auto &dog : dogs) {
      boost::json::object player;
      player[MakeBoostSV(JsonAttributes::NAME)] = dog.second;
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
    return MakeWrongMethodError(HttpResponseConstants::GET_HEAD_EXPECTED,
                                HttpResponseConstants::ALLOWED_GET_HEAD,
                                req.version());
  }

  std::variant<JsonResponse, std::shared_ptr<app::Token>> result = GetToken(req);

  if (result.index() == 1) {
    std::shared_ptr<app::Token> token = std::get<std::shared_ptr<app::Token>>(result);

    auto game_state = app_.GetGameState(*token);
    boost::json::object players;

    for (auto data = game_state.first.rbegin(); data != game_state.first.rend(); ++data) {
      boost::json::object params;

      boost::json::array pos_array{data->pos.x, data->pos.y};
      boost::json::array speed_array{data->speed.horizontal, data->speed.vertical};
      boost::json::array bag_array;

      for (auto &thing : data->bag) {
        boost::json::object thing_obj;
        thing_obj[MakeBoostSV(JsonAttributes::ID)] = thing.id;
        thing_obj[MakeBoostSV(JsonAttributes::TYPE)] = thing.type;
        bag_array.push_back(thing_obj);
      }

      params[MakeBoostSV(JsonAttributes::POS)] = pos_array;
      params[MakeBoostSV(JsonAttributes::SPEED)] = speed_array;
      params[MakeBoostSV(JsonAttributes::DIR)] = data->dir;
      params[MakeBoostSV(JsonAttributes::BAG)] = bag_array;
      params[MakeBoostSV(JsonAttributes::SCORE)] = data->score;



      players[std::to_string(*data->id)] = params;
    }

    boost::json::object res_object;

    res_object[MakeBoostSV(JsonAttributes::PLAYERS)] = players;


    boost::json::object lost_objects;
    auto loot = game_state.second;

    int i = 0;

    for(auto object : loot) {
      boost::json::object params;

      params[MakeBoostSV(JsonAttributes::TYPE)] = object.second.type;
      params[MakeBoostSV(JsonAttributes::POS)] = {object.second.x, object.second.y};

      lost_objects[MakeBoostSV(std::to_string(i++))] = params;
    }

    res_object[MakeBoostSV(JsonAttributes::LOST_OBJECTS)] = lost_objects;


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
    return MakeWrongMethodError(HttpResponseConstants::POST_EXPECTED,
                                HttpResponseConstants::ALLOWED_POST,
                                req.version());
  }

  if (req.count(http::field::content_type) == 0 || req.at(http::field::content_type) != ContentType::APPLICATION_JSON) {
    return MakeBadRequestError(HttpResponseConstants::MESSAGE_INVALID_CONTENT_TYPE, req.version());
  }

  std::variant<JsonResponse, std::shared_ptr<app::Token>> result = GetToken(req);

  if (result.index() == 1) {
    std::shared_ptr<app::Token> token = std::get<std::shared_ptr<app::Token>>(result);

    json::value value, move;

    try {
      value = json::parse(req.body());
      move = value.at(MakeBoostSV(JsonAttributes::MOVE));
    } catch (std::exception &e) {
      return MakeBadRequestError(HttpResponseConstants::PARSE_ACTION_ERROR, req.version());
    }

    std::string dir(move.as_string());

    if (dir.size() > 1 || (dir.size() == 1 && std::string("LRUD").find(dir) == std::string::npos)) {
      return MakeBadRequestError(HttpResponseConstants::PARSE_ACTION_ERROR, req.version());
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
    return MakeWrongMethodError(HttpResponseConstants::POST_EXPECTED,
                                HttpResponseConstants::ALLOWED_POST,
                                req.version());
  }

  if (req.count(http::field::content_type) == 0 || req.at(http::field::content_type) != ContentType::APPLICATION_JSON) {
    return MakeBadRequestError(HttpResponseConstants::MESSAGE_INVALID_CONTENT_TYPE, req.version());
  }

  uint64_t time_delta;

  try {
    auto value = json::parse(req.body());
    time_delta = value.at(MakeBoostSV(JsonAttributes::TIME_DELTA)).as_int64();
  } catch (std::exception &e) {
    return MakeBadRequestError(HttpResponseConstants::PARSE_TICK_ERROR, req.version());
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
  } else if (target == SystemPaths::TICK && manual_tick) {
    return HandleTickRequest(req);
  }

  boost::json::object error_obj;

  error_obj[MakeBoostSV(JsonAttributes::CODE)] = MakeBoostSV(HttpResponseConstants::CODE_400);
  error_obj[MakeBoostSV(JsonAttributes::MESSAGE)] = MakeBoostSV(HttpResponseConstants::MESSAGE_INVALID_ENDPOINT);

  return MakeJsonResponse(http::status::bad_request,
                          boost::json::serialize(error_obj),
                          req.version(),
                          req.keep_alive());
}

} // namespace http_handler
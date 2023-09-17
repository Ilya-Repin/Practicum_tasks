#include "request_handler.h"

namespace http_handler {
StringResponse MakeStringResponse(http::status status, std::string_view body, size_t http_version,
                                  bool keep_alive,
                                  std::string_view content_type) {
  StringResponse response(status, http_version);
  response.set(http::field::content_type, content_type);
  response.body() = body;
  response.content_length(body.size());
  response.keep_alive(keep_alive);

  return response;
}

std::string RequestHandler::DecodeURL(std::string_view target) {
  std::string path;
  path.reserve(target.size());

  for (size_t i = 0; i < target.size(); ++i) {
    char letter = target[i];

    if (letter == '%') {
      ++i;
      std::string substr(target.substr(i, 2));
      int code = std::stoi(substr, nullptr, 16);
      ++i;
      letter = static_cast<char>(code);
    }

    path += letter;
  }

  return path;
}

fs::path RequestHandler::GetPath(std::string_view url) {
  return fs::weakly_canonical(static_path_.string() + DecodeURL(url));
}

bool RequestHandler::CheckFileInRoot(fs::path &path) {
  int result = path.string().compare(0, static_path_.string().length(), static_path_.string());
  return result == 0;
}

bool RequestHandler::CheckFileExistence(fs::path &path) {
  return fs::exists(path);
}

std::shared_ptr<boost::json::array> RequestHandler::MakeMapsArr() {
  boost::json::array maps_arr;
  auto maps = game_.GetMaps();

  for (auto map : maps) {
    boost::json::object map_obj;
    map_obj[MakeBoostSV(JsonAttributes::ID)] = *map.GetId();
    map_obj[MakeBoostSV(JsonAttributes::NAME)] = map.GetName();
    maps_arr.push_back(std::move(map_obj));
  }

  return std::make_shared<boost::json::array>(maps_arr);
}

std::shared_ptr<boost::json::array> RequestHandler::MakeRoadsArr(const model::Map *map) {
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

std::shared_ptr<boost::json::array> RequestHandler::MakeBuildingsArr(const model::Map *map) {
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

std::shared_ptr<boost::json::array> RequestHandler::MakeOfficesArr(const model::Map *map) {
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

void RequestHandler::ConfigureJsonMap(boost::json::object &map_obj, const model::Map *map) {
  map_obj[MakeBoostSV(JsonAttributes::ID)] = *map->GetId();
  map_obj[MakeBoostSV(JsonAttributes::NAME)] = map->GetName();

  std::shared_ptr<boost::json::array> roads = MakeRoadsArr(map);
  map_obj[MakeBoostSV(JsonAttributes::ROADS)] = *roads;

  std::shared_ptr<boost::json::array> buildings = MakeBuildingsArr(map);
  map_obj[MakeBoostSV(JsonAttributes::BUILDINGS)] = *buildings;

  std::shared_ptr<boost::json::array> offices = MakeOfficesArr(map);
  map_obj[MakeBoostSV(JsonAttributes::OFFICES)] = *offices;
}

}  // namespace http_handler

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

std::shared_ptr<boost::json::array> RequestHandler::MakeMapsArr() {
  boost::json::array maps_arr;
  auto maps = game_.GetMaps();

  for (auto map : maps) {
    boost::json::object map_obj;
    map_obj["id"] = *map.GetId();
    map_obj["name"] = map.GetName();
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
      road_obj["x0"] = road.GetStart().x;
      road_obj["y0"] = road.GetStart().y;
      road_obj["x1"] = road.GetEnd().x;
    } else {
      road_obj["x0"] = road.GetStart().x;
      road_obj["y0"] = road.GetStart().y;
      road_obj["y1"] = road.GetEnd().y;
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

    building_obj["x"] = building.GetBounds().position.x;
    building_obj["y"] = building.GetBounds().position.y;
    building_obj["w"] = building.GetBounds().size.width;
    building_obj["h"] = building.GetBounds().size.height;

    buildings_arr.push_back(std::move(building_obj));
  }

  return std::make_shared<boost::json::array>(buildings_arr);
}

std::shared_ptr<boost::json::array> RequestHandler::MakeOfficesArr(const model::Map *map) {
  boost::json::array offices_arr;
  auto offices = map->GetOffices();

  for (auto office : offices) {
    boost::json::object office_obj;

    office_obj["id"] = *office.GetId();
    office_obj["x"] = office.GetPosition().x;
    office_obj["y"] = office.GetPosition().y;
    office_obj["offsetX"] = office.GetOffset().dx;
    office_obj["offsetY"] = office.GetOffset().dy;

    offices_arr.push_back(std::move(office_obj));
  }

  return std::make_shared<boost::json::array>(offices_arr);
}

void RequestHandler::ConfigureJsonMap(boost::json::object &map_obj, const model::Map *map) {
  map_obj["id"] = *map->GetId();
  map_obj["name"] = map->GetName();

  std::shared_ptr<boost::json::array> roads = MakeRoadsArr(map);
  map_obj["roads"] = *roads;

  std::shared_ptr<boost::json::array> buildings = MakeBuildingsArr(map);
  map_obj["buildings"] = *buildings;

  std::shared_ptr<boost::json::array> offices = MakeOfficesArr(map);
  map_obj["offices"] = *offices;
}

}  // namespace http_handler

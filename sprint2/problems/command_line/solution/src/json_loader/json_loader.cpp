#include "json_loader.h"

namespace json_loader {

namespace json = boost::json;
using namespace model;

std::string GetStringValue(const json::object &obj, std::string_view attribute) {
  return obj.at(MakeBoostSV(attribute)).as_string().data();
}

int GetIntValue(json::object &obj, std::string_view attribute) {
  return obj.at(MakeBoostSV(attribute)).as_int64();
}

double GetDoubleValue(json::object &obj, std::string_view attribute) {
  return obj.at(MakeBoostSV(attribute)).as_double();
}

std::vector<Road> GetRoads(json::object &map) {
  auto roads_array = map.at(MakeBoostSV(JsonAttributes::ROADS)).as_array();
  std::vector<Road> roads;

  for (auto element : roads_array) {
    auto road_object = element.as_object();

    Point start{GetIntValue(road_object, JsonAttributes::X0), GetIntValue(road_object, JsonAttributes::Y0)};

    bool is_horizontal = road_object.contains(MakeBoostSV(JsonAttributes::X1));
    Coord end = GetIntValue(road_object, (is_horizontal ? JsonAttributes::X1 : JsonAttributes::Y1));

    if (is_horizontal) {
      roads.emplace_back(Road::HORIZONTAL, start, end);
    } else {
      roads.emplace_back(Road::VERTICAL, start, end);
    }
  }

  return roads;
}

std::vector<Building> GetBuildings(json::object &map) {
  auto buildings_array = map.at(MakeBoostSV(JsonAttributes::BUILDINGS)).as_array();
  std::vector<Building> buildings;

  for (auto element : buildings_array) {
    auto building_obj = element.as_object();

    Size size{GetIntValue(building_obj, JsonAttributes::WIDTH), GetIntValue(building_obj, JsonAttributes::HEIGHT)};
    Point point{GetIntValue(building_obj, JsonAttributes::X), GetIntValue(building_obj, JsonAttributes::Y)};

    Rectangle bounds{point, size};

    buildings.emplace_back(bounds);
  }

  return buildings;
}

std::vector<Office> GetOffices(json::object &map) {
  auto offices_array = map.at(MakeBoostSV(JsonAttributes::OFFICES)).as_array();
  std::vector<Office> offices;

  for (auto element : offices_array) {
    auto office_obj = element.as_object();

    Office::Id id{GetStringValue(office_obj, JsonAttributes::ID)};
    Point position{GetIntValue(office_obj, JsonAttributes::X), GetIntValue(office_obj, JsonAttributes::Y)};
    Offset offset{GetIntValue(office_obj, JsonAttributes::OFFSET_X), GetIntValue(office_obj, JsonAttributes::OFFSET_Y)};

    offices.emplace_back(id, position, offset);
  }

  return offices;
}

std::string GetJson(const std::filesystem::path &json_path) {
  std::ifstream json_file(json_path);

  if (!json_file.is_open()) {
    throw std::runtime_error("File Not Found");
  }

  std::string json_content((std::istreambuf_iterator<char>(json_file)), std::istreambuf_iterator<char>());
  json_file.close();

  return json_content;
}

Game LoadGame(const std::filesystem::path &json_path) {
  Game game;

  auto value = json::parse(GetJson(json_path));
  auto maps_array = value.at(MakeBoostSV(JsonAttributes::MAPS)).as_array();

  double default_dog_speed = 1.0;

  if (value.as_object().contains(MakeBoostSV(JsonAttributes::DEFAULT_DOG_SPEED))) {
    default_dog_speed = GetDoubleValue(value.as_object(), JsonAttributes::DEFAULT_DOG_SPEED);
  }

  for (auto &element : maps_array) {
    auto map_object = element.as_object();
    double dog_speed = default_dog_speed;

    if (map_object.contains(MakeBoostSV(JsonAttributes::DOG_SPEED))) {
      dog_speed = GetDoubleValue(map_object, JsonAttributes::DOG_SPEED);
    }

    Map map(Map::Id{GetStringValue(map_object, JsonAttributes::ID)},
            GetStringValue(map_object, JsonAttributes::NAME),
            dog_speed);

    for (const auto &road : GetRoads(map_object)) {
      map.AddRoad(road);
    }

    for (const auto &building : GetBuildings(map_object)) {
      map.AddBuilding(building);
    }

    for (const auto &office : GetOffices(map_object)) {
      map.AddOffice(office);
    }

    game.AddMap(map);
  }

  return game;
}

}  // namespace json_loader
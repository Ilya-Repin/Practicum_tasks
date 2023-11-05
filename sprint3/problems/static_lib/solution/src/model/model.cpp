#include "model.h"
#include <stdexcept>
#include <iostream>

namespace model {

using namespace constants;
using namespace std::literals;

void Game::AddMap(const Map &map) {

  const size_t index = maps_.size();

  if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
    throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
  } else {
    try {
      maps_.emplace_back(std::make_shared<Map>(std::move(map)));

    } catch (...) {

      map_id_to_index_.erase(it);
      throw;
    }
  }
}

void Map::AddOffice(const Office &office) {
  if (warehouse_id_to_index_.contains(office.GetId())) {
    throw std::invalid_argument("Duplicate warehouse");
  }

  const size_t index = offices_.size();
  Office &o = offices_.emplace_back(std::move(office));

  try {
    warehouse_id_to_index_.emplace(o.GetId(), index);
  } catch (...) {
    offices_.pop_back();
    throw;
  }
}

double Map::GetDogSpeed() const noexcept {
  return dog_speed_;
}

std::pair<bool, Position> Map::CalculateNewPosition(Position pos, Velocity speed, uint64_t time_delta) const {
  if (speed == Velocity{0.0, 0.0}) {
    return {false, pos};
  }

  Position expected_position = pos + CalculateShift(speed, time_delta);
  Position valid_position = ValidatePosition(pos, expected_position);

  bool need_to_stop_dog = (valid_position != expected_position);

  return {need_to_stop_dog, valid_position};
}

Position Map::CalculateShift(Velocity speed, uint64_t time_delta) const {
  return Position{speed.horizontal * (static_cast<double>(time_delta) / 1000.0),
                  speed.vertical * (static_cast<double>(time_delta) / 1000.0)};
}

Position Map::ValidatePosition(Position start, Position finish) const {
  Position final_destination = finish;

  // В одной точке могут сходиться до четырёх дорог
  std::vector<const Road *> roads = FindRoadsByPosition(start);

  bool is_valid = false;

  for (auto road : roads) {
    if (road->Contain(finish)) {
      is_valid = true;
    }
  }

  if (roads.size() == 0) {
    return final_destination;
  }

  // Если пункт назначения не входит ни в одну из дорог
  if (!is_valid) {
    double min_dist = std::numeric_limits<double>::max();

    for (auto &road : roads) {
      Position point = road->GetClosestPoint(finish);

      double dist = Distance(point, finish);

      if (dist < min_dist) {
        min_dist = dist;
        final_destination = point;
      }
    }
  }

  return final_destination;
}

Map::Crossroad Map::FindRoadsByPosition(Position pos) const {
  Crossroad roads;

  for (auto &road : roads_) {
    if (road.Contain(pos)) {
      roads.push_back(&road);
    }
  }

  return roads;
}

bool Road::Contain(Position pos) const noexcept {
  double left_border = std::min(start_.x, end_.x) - GameSettings::LANE_WIDTH;
  double right_border = std::max(start_.x, end_.x) + GameSettings::LANE_WIDTH;
  double upper_border = std::min(start_.y, end_.y) - GameSettings::LANE_WIDTH;
  double lower_border = std::max(start_.y, end_.y) + GameSettings::LANE_WIDTH;

  return ((left_border <= pos.x) && (pos.x <= right_border) && (pos.y <= lower_border) && (upper_border <= pos.y));
}

Position Road::GetClosestPoint(Position pos) const noexcept {
  double left_border = std::min(start_.x, end_.x) - GameSettings::LANE_WIDTH;
  double right_border = std::max(start_.x, end_.x) + GameSettings::LANE_WIDTH;
  double upper_border = std::min(start_.y, end_.y) - GameSettings::LANE_WIDTH;
  double lower_border = std::max(start_.y, end_.y) + GameSettings::LANE_WIDTH;

  Position closest_point;

//  WITHOUT_HORIZONTAL_DEVIATION
  if ((left_border <= pos.x) && pos.x <= right_border) {
    closest_point.x = pos.x;
    closest_point.y = (pos.y <= upper_border ? upper_border : lower_border);
  }

//  WITHOUT_VERTICAL_DEVIATION
  else if (upper_border <= pos.y && pos.y <= lower_border) {
    closest_point.y = pos.y;
    closest_point.x = ((pos.x <= left_border) ? left_border : right_border);
  } else {
    closest_point.x = (pos.x <= left_border ? left_border : right_border);
    closest_point.y = (pos.y <= upper_border ? upper_border : lower_border);
  }

  return closest_point;
}

}  // namespace model
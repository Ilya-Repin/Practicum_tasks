#include "model.h"
#include <memory>
#include <stdexcept>
#include <limits>
#include <iostream>

namespace model {
using namespace std::literals;

void Map::AddOffice(const Office &office) {
  if (warehouse_id_to_index_.contains(office.GetId())) {
    throw std::invalid_argument("Duplicate warehouse");
  }

  const size_t index = offices_.size();
  Office &o = offices_.emplace_back(std::move(office));
  try {
    warehouse_id_to_index_.emplace(o.GetId(), index);
  } catch (...) {
    // Удаляем офис из вектора, если не удалось вставить в unorderedmap
    offices_.pop_back();
    throw;
  }
}

double Map::GetDogSpeed() const {
  return dog_speed_;
}

std::pair<bool, Position> Map::CalculateNewPosition(Position pos, Velocity speed, uint64_t time_delta) const {
  if (speed == Velocity{0.0, 0.0}) {
//    std::cout << "Ну скорость нулевая-то, что ты от меня хочешь?" << std::endl;

    return {false, pos};
  }

//  std::cout << "Начинаем вычислять!" << std::endl;
//  std::cout << "Вычисленный сдвиг " << CalculateShift(speed, time_delta).x << " " << CalculateShift(speed, time_delta).y
//            << std::endl;
  Position expected_position = pos + CalculateShift(speed, time_delta);
//  std::cout << "Ожидаемая позиция" << expected_position.x << " " << expected_position.y << std::endl;

  Position valid_position = ValidatePosition(pos, expected_position);
//  std::cout << "После валидации " << valid_position.x << " " << valid_position.y << std::endl;

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
//  std::cout << "Здесь сходятся столько-то дорог" << roads.size() << std::endl;
  bool is_valid = false;

  for (auto road : roads) {
    if (road->Contain(finish)) {
      is_valid = true;
    }
  }

//  std::cout << "Пермещение валидно (bool) " << is_valid << std::endl;
  if (roads.size() == 0) {
    std::cout << "Собака вне тротуара!" <<std::endl;
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

  // TODO Тут бы более эффективный алгоритм
  for (auto &road : roads_) {
    if (road.Contain(pos)) {
      roads.push_back(&road);
    }
  }

  return roads;
}

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
bool Road::Contain(Position pos) const noexcept {

  double left_border = std::min(start_.x, end_.x) - 0.4;
  double right_border = std::max(start_.x, end_.x) + 0.4;
  double upper_border = std::min(start_.y, end_.y) - 0.4;
  double lower_border = std::max(start_.y, end_.y) + 0.4;

return ((left_border <= pos.x) && (pos.x <= right_border) && (pos.y <= lower_border) && (upper_border <= pos.y));
}

Position Road::GetClosestPoint(Position pos) const noexcept {
  double left_border = std::min(start_.x, end_.x) - 0.4;
  double right_border = std::max(start_.x, end_.x) + 0.4;
  double upper_border = std::min(start_.y, end_.y) - 0.4;
  double lower_border = std::max(start_.y, end_.y) + 0.4;

  Position closest_point;

//  WITHOUT_HORIZONTAL_DEVIATION
  if ((left_border <= pos.x) && pos.x <= right_border) {
    closest_point.x = pos.x;
    closest_point.y = (pos.y <= upper_border  ? upper_border : lower_border);
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
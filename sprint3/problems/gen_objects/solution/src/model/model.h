#pragma once

#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>
#include <map>
#include <chrono>

#include "dog.h"
#include "../constants_storage.h"
#include "../extra_data.h"

namespace model {

using Dimension = int;
using Coord = Dimension;

struct Point {
  Coord x, y;
};

struct Size {
  Dimension width, height;
};

struct Rectangle {
  Point position;
  Size size;
};

struct Offset {
  Dimension dx, dy;
};

struct LootGeneratorConfig {
  std::chrono::milliseconds  period;
  double probability;
};

class Road {
  struct HorizontalTag {
    HorizontalTag() = default;
  };

  struct VerticalTag {
    VerticalTag() = default;
  };

 public:
  constexpr static HorizontalTag HORIZONTAL{};
  constexpr static VerticalTag VERTICAL{};

  Road(HorizontalTag, Point start, Coord end_x) noexcept
      : start_{start}, end_{end_x, start.y} {
  }

  Road(VerticalTag, Point start, Coord end_y) noexcept
      : start_{start}, end_{start.x, end_y} {
  }

  bool IsHorizontal() const noexcept {
    return start_.y == end_.y;
  }

  bool IsVertical() const noexcept {
    return start_.x == end_.x;
  }

  Point GetStart() const noexcept {
    return start_;
  }

  Point GetEnd() const noexcept {
    return end_;
  }

  bool Contain(Position pos) const noexcept;

  Position GetClosestPoint(Position pos) const noexcept;

 private:
  Point start_;
  Point end_;
};

class Building {
 public:
  explicit Building(Rectangle bounds) noexcept
      : bounds_{bounds} {
  }

  const Rectangle &GetBounds() const noexcept {
    return bounds_;
  }

 private:
  Rectangle bounds_;
};

class Office {
 public:
  using Id = util::Tagged<std::string, Office>;

  Office(Id id, Point position, Offset offset) noexcept
      : id_{std::move(id)}, position_{position}, offset_{offset} {
  }

  const Id &GetId() const noexcept {
    return id_;
  }

  Point GetPosition() const noexcept {
    return position_;
  }

  Offset GetOffset() const noexcept {
    return offset_;
  }

 private:
  Id id_;
  Point position_;
  Offset offset_;
};

class Map {
 public:
  using Id = util::Tagged<std::string, Map>;
  using Roads = std::vector<Road>;
  using Buildings = std::vector<Building>;
  using Offices = std::vector<Office>;

  Map(Id id, std::string name, double speed, ExtraData  extra_data) noexcept
      : id_(std::move(id)), name_(std::move(name)), dog_speed_(speed), extra_data_(std::move(extra_data)) {
  }

  const Id &GetId() const noexcept {
    return id_;
  }

  const std::string &GetName() const noexcept {
    return name_;
  }

  const Buildings &GetBuildings() const noexcept {
    return buildings_;
  }

  const Roads &GetRoads() const noexcept {
    return roads_;
  }

  const Offices &GetOffices() const noexcept {
    return offices_;
  }

  const ExtraData &GetExtraData() const noexcept {
    return extra_data_;
  }

  const int GetLootTypesAmount() const noexcept {
    return extra_data_.GetLootTypesAmount();
  }

  void AddRoad(const Road &road) {
    roads_.emplace_back(road);
  }

  void AddBuilding(const Building &building) {
    buildings_.emplace_back(building);
  }

  void AddOffice(const Office &office);

  double GetDogSpeed() const noexcept;

  std::pair<bool, Position> CalculateNewPosition(Position pos, Velocity speed, uint64_t time_delta) const;

 private:
  using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;
  using Crossroad = std::vector<const Road *>;

  Id id_;
  double dog_speed_;
  std::string name_;

  Roads roads_;
  Buildings buildings_;
  Offices offices_;
  ExtraData extra_data_;

  OfficeIdToIndex warehouse_id_to_index_;

  // Methods
  Position CalculateShift(Velocity speed, uint64_t time_delta) const;
  Position ValidatePosition(Position start, Position finish) const;
  Crossroad FindRoadsByPosition(Position pos) const;
};

class Game {
 public:
  using Maps = std::vector<std::shared_ptr<Map>>;

  Game (LootGeneratorConfig config) : config_(config) {
  }

  void AddMap(const Map &map);

  const Maps &GetMaps() const noexcept {
    return maps_;
  }

  const LootGeneratorConfig GetLootGeneratorConfig() const noexcept {
    return config_;
  }

  std::shared_ptr<Map> FindMap(const Map::Id &id) const noexcept {
    if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
      return maps_.at(it->second);
    }

    return nullptr;
  }

 private:
  using MapIdHasher = util::TaggedHasher<Map::Id>;
  using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

  Maps maps_;
  MapIdToIndex map_id_to_index_;
  LootGeneratorConfig config_;
};

}  // namespace model

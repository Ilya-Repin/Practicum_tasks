#pragma once
#include <iostream>
#include "model.h"
#include "Loot/loot_generator.h"
#include "collision_detector/collision_detector.h"
namespace model {

struct Loot {
  int type;
  uint64_t value;
  double x;
  double y;
};

class GameSession {
 public:
  using Id = util::Tagged<std::uint64_t, GameSession>;
  using SessionIdHasher = util::TaggedHasher<Id>;
  using DogIdHasher = util::TaggedHasher<Dog::Id>;
  using Dogs = std::unordered_map<Dog::Id, std::shared_ptr<Dog>, DogIdHasher>;
  using Loots = std::unordered_map<uint64_t, model::Loot>;

  // Constructors
  GameSession(const model::Map &map, Id id, bool is_spawn_point_random, const LootGeneratorConfig &config)
      : map_(map),
        id_(id),
        is_spawn_point_random_(is_spawn_point_random),
        loot_generator_(config.period, config.probability) {
    auto start_point = map.GetRoads()[0].GetStart();
    start_position = Position({static_cast<double>(start_point.x), static_cast<double>(start_point.y)});
  }

  GameSession(const model::Map &map, Id id, bool is_spawn_point_random, const LootGeneratorConfig &config, Dogs&& dogs, Loots &&loots)
      : map_(map),
        id_(id),
        is_spawn_point_random_(is_spawn_point_random),
        loot_generator_(config.period, config.probability),
        dogs_(std::move(dogs)),
        loots_(std::move(loots)) {
    auto start_point = map.GetRoads()[0].GetStart();
    start_position = Position({static_cast<double>(start_point.x), static_cast<double>(start_point.y)});
  }

  // Getters
  const Dogs &GetDogs() const noexcept;
  const Id &GetId() const noexcept;
  const Map::Id &GetMapId() const noexcept;
  const Loots &GetLoots() const noexcept;
  std::vector<std::pair<uint64_t, std::string>> GetPlayersData() const;
  Position GetRandomPosition() const;

  // Methods
  std::shared_ptr<model::Dog> CreateDog(std::string &dog_name);
  void Tick(uint64_t time_delta);

  bool CanAddPlayer() const noexcept;

  // Only for serialization


 private:
  Id id_;
  const Map &map_;
  Dogs dogs_;
  Loots loots_;
  loot_gen::LootGenerator loot_generator_;

  uint64_t players_amount_ = 0;
  uint64_t loots_amount_ = 0;
  bool is_spawn_point_random_;
  Position start_position;

  size_t MAX_PLAYERS_AMOUNT = 16;

  // Private Methods
  void MoveDogs(uint64_t time_delta);
  void AddLoot(uint64_t time_delta);
  void EraseLoot(uint64_t id);
  void HandleCollisions();

  // Predicates
  bool IsLoot(size_t item_id);
};

} // namespace model
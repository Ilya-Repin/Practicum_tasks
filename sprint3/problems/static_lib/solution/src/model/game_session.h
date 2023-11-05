#pragma once
#include <iostream>
#include "model.h"
#include "loot_generator.h"

namespace model {

struct Loot {
  int type;
  double x;
  double y;
};

class GameSession {
 public:
  using Id = util::Tagged<std::uint64_t, GameSession>;
  using SessionIdHasher = util::TaggedHasher<Id>;
  using DogIdHasher = util::TaggedHasher<Dog::Id>;
  using Dogs = std::unordered_map<Dog::Id, std::shared_ptr<Dog>, DogIdHasher>;
  using Loots = std::vector<Loot>;

  GameSession(const model::Map &map, Id id, bool is_spawn_point_random, const LootGeneratorConfig& config)
      : map_(map), id_(id), is_spawn_point_random_(is_spawn_point_random), loot_generator_(config.period, config.probability) {
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

 private:
  Id id_;
  const Map &map_;
  Dogs dogs_;
  Loots loots_;
  loot_gen::LootGenerator loot_generator_;

  uint64_t players_amount_ = 0;
  bool is_spawn_point_random_;
  Position start_position;

  size_t MAX_PLAYERS_AMOUNT = 16;
};

} // model

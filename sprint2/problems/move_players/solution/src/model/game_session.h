#pragma once
#include <unordered_map>
#include <iostream>
#include <memory>
#include "../tagged.h"
#include "dog.h"
#include "model.h"

namespace model {

class GameSession {
 public:
  using Id = util::Tagged<std::uint64_t, GameSession>;
  using SessionIdHasher = util::TaggedHasher<Id>;
  using DogIdHasher = util::TaggedHasher<Dog::Id>;
  using Dogs = std::unordered_map<Dog::Id, std::shared_ptr<Dog>, DogIdHasher>;

  GameSession(const model::Map &map, Id id) : map_(map), id_(id) {
  }

  // Add
  std::shared_ptr<model::Dog> CreateDog(std::string& dog_name);


  // Getters
  const Dogs &GetDogs() const noexcept;
  const Id &GetId();
  const Map::Id &GetMapId();
  std::vector<std::pair< uint64_t, std::string>> GetPlayersData();


  bool CanAddPlayer();

 private:
  Id id_;
  const Map &map_;
  Dogs dogs_;
  uint64_t players_amount_ = 0;

  const static size_t MAX_PLAYERS_AMOUNT = 16; // TODO Может убрать???
};

} // model

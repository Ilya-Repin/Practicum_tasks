#include "game_session.h"

namespace model {

std::shared_ptr<model::Dog> GameSession::CreateDog(std::string &dog_name) {
  std::shared_ptr<Dog> dog_ptr = std::make_shared<Dog>(dog_name);
  dogs_.insert({dog_ptr->GetId(), dog_ptr});

  ++players_amount_;

  return dog_ptr;
}

const GameSession::Id &GameSession::GetId() {
  return id_;
}

const Map::Id &GameSession::GetMapId() {
  return map_.GetId();
}

const GameSession::Dogs &GameSession::GetDogs() const noexcept {
  return dogs_;
}

bool GameSession::CanAddPlayer() {
  return players_amount_ <= MAX_PLAYERS_AMOUNT;
}

std::vector<std::pair< uint64_t, std::string>> GameSession::GetPlayersData() {
  std::vector<std::pair<uint64_t, std::string>> data;
  for(auto& dog : dogs_) {
    data.emplace_back(*dog.first, dog.second->GetName());
  }

  return data;
}

} // model
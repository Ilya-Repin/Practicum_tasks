#include "game_session.h"
#include <random>

namespace model {

std::shared_ptr<model::Dog> GameSession::CreateDog(std::string &dog_name) {
  auto position = start_position;

  if (is_spawn_point_random_) {
    position = GetRandomPosition();
  }

  std::shared_ptr<Dog> dog_ptr = std::make_shared<Dog>(dog_name, map_.GetDogSpeed(), position);
  dogs_.insert({dog_ptr->GetId(), dog_ptr});

  ++players_amount_;

  return dog_ptr;
}

const GameSession::Id &GameSession::GetId() const noexcept {
  return id_;
}

const Map::Id &GameSession::GetMapId() const noexcept {
  return map_.GetId();
}

const GameSession::Dogs &GameSession::GetDogs() const noexcept {
  return dogs_;
}

bool GameSession::CanAddPlayer() const noexcept {
  return players_amount_ < MAX_PLAYERS_AMOUNT;
}

std::vector<std::pair<uint64_t, std::string>> GameSession::GetPlayersData() const {
  std::vector<std::pair<uint64_t, std::string>> data;

  for (auto &dog : dogs_) {
    data.emplace_back(*dog.first, dog.second->GetName());
  }

  return data;
}
void GameSession::Tick(uint64_t time_delta) {
  int new_loot_amount = loot_generator_.Generate(std::chrono::milliseconds(time_delta), loots_.size(), dogs_.size());

  for(int i = 0; i < new_loot_amount; ++i) {
    int random_type = rand() % map_.GetLootTypesAmount();
    auto spawn_position = GetRandomPosition();
    loots_.push_back({random_type, spawn_position.x, spawn_position.y});
  }

  for (auto &dog : dogs_) {
    std::pair<bool, Position> instructions_for_dog =
        map_.CalculateNewPosition(dog.second->GetPosition(), dog.second->GetVelocity(), time_delta);

    dog.second->SetPosition(instructions_for_dog.second);

    if (instructions_for_dog.first) {
      dog.second->Stop();
    }
  }
}

Position GameSession::GetRandomPosition() const {
  auto roads = map_.GetRoads();

  size_t random_index = rand() % roads.size();
  auto chosen_road = roads[random_index];

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis_x(chosen_road.GetStart().x, chosen_road.GetEnd().x);
  std::uniform_real_distribution<> dis_y(chosen_road.GetStart().y, chosen_road.GetEnd().y);

  return {dis_x(gen), dis_y(gen)};
}
const GameSession::Loots &GameSession::GetLoots() const noexcept {
  return loots_;
}

} // model
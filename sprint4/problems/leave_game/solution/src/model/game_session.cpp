#include "game_session.h"
#include <random>

namespace model {

std::shared_ptr<model::Dog> GameSession::CreateDog(std::string &dog_name) {
  auto position = start_position;

  if (is_spawn_point_random_) {
    position = GetRandomPosition();
  }

  std::shared_ptr<Dog> dog_ptr = std::make_shared<Dog>(dog_name, map_.GetDogSpeed(), position, map_.GetBagCapacity());
  dogs_.insert({dog_ptr->GetId(), dog_ptr});

  ++players_amount_;

  return dog_ptr;
}

std::vector<std::shared_ptr<Dog>> GameSession::Tick(uint64_t time_delta) {
  auto retired_dogs = UpdateDogState(time_delta);
  HandleCollisions();
  AddLoot(time_delta);

  return retired_dogs;
}

std::vector<std::shared_ptr<Dog>> GameSession::UpdateDogState(uint64_t time_delta) {
  std::vector<std::shared_ptr<Dog>> retired;

  for (auto &dog : dogs_) {
    auto dog_move = map_.CalculateNewPosition(dog.second->GetPosition(), dog.second->GetVelocity(), time_delta);
    dog.second->SetPosition(dog_move.second);
    dog.second->UpdateTime(time_delta);

    if (dog_move.first) {
      dog.second->Stop();
    }

    if (dog.second->GetIdleTime() >= dog_retirement_time_) {
      retired.push_back(dog.second);
    }
  }

  for (auto &dog : retired) {
    dogs_.erase(dog->GetId());
  }

  return retired;
}

void GameSession::AddLoot(uint64_t time_delta) {
  auto new_loot_amount = loot_generator_.Generate(std::chrono::milliseconds(time_delta), loots_.size(), dogs_.size());

  for (int i = 0; i < new_loot_amount; ++i) {
    int random_type = rand() % map_.GetLootTypesAmount();
    auto spawn_position = GetRandomPosition();
    loots_.insert({loots_amount_++, {random_type, map_.GetValues()[random_type], spawn_position.x, spawn_position.y}});
  }
}

void GameSession::HandleCollisions() {
  using namespace collision_detector;

  ItemGatherer item_gatherer;
  std::vector<Dog::Id> index_to_dog_id;

  for (auto &dog : dogs_) {
    auto position = dog.second->GetPosition();
    auto previous_position = dog.second->GetPreviousPosition();

    item_gatherer.AddGatherer(Gatherer{{position.x, position.y},
                                       {previous_position.x, previous_position.y},
                                       constants::GameSettings::DOG_WIDTH});

    index_to_dog_id.push_back(dog.first);
  }

  for (uint64_t i = 0; i < loots_amount_; ++i) {
    item_gatherer.AddItem(Item{{loots_[i].x, loots_[i].y}, constants::GameSettings::LOOT_WIDTH});
  }

  for (auto &office : map_.GetOffices()) {
    std::pair<double, double> coord{office.GetPosition().x, office.GetPosition().y};
    item_gatherer.AddItem({{coord.first, coord.second}, constants::GameSettings::BASE_WIDTH});
  }

  auto gathering_events = FindGatherEvents(item_gatherer);

  for (auto &event : gathering_events) {
    auto dog = dogs_[index_to_dog_id[event.gatherer_id]];

    if (IsLoot(event.item_id)) {
      if (!dog->IsBagFull()) {
        dog->TakeLoot({event.item_id, loots_[event.item_id].type});
        EraseLoot(event.item_id);
      }
    } else {
      dog->UnloadBag(map_.GetValues());
    }
  }

}

void GameSession::EraseLoot(uint64_t id) {
  --loots_amount_;
  loots_.erase(id);
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

std::vector<std::pair<uint64_t, std::string>> GameSession::GetPlayersData() const {
  std::vector<std::pair<uint64_t, std::string>> data;

  for (auto &dog : dogs_) {
    data.emplace_back(*dog.first, dog.second->GetName());
  }

  return data;
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

bool GameSession::IsLoot(size_t item_id) {
  return item_id < loots_amount_;
}

bool GameSession::CanAddPlayer() const noexcept {
  return players_amount_ < MAX_PLAYERS_AMOUNT;
}

} // namespace model
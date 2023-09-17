#include "game_session.h"

namespace model {

std::shared_ptr<model::Dog> GameSession::CreateDog(std::string &dog_name) {
  std::shared_ptr<Dog> dog_ptr = std::make_shared<Dog>(dog_name, map_.GetDogSpeed(), start_position);//TODO start_position сделай рандомным
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

std::vector<std::pair<uint64_t, std::string>> GameSession::GetPlayersData() {
  std::vector<std::pair<uint64_t, std::string>> data;

  for (auto &dog : dogs_) {
    data.emplace_back(*dog.first, dog.second->GetName());
  }

  return data;
}
void GameSession::Tick(uint64_t time_delta) {
  for (auto &dog : dogs_) {
//    std::cout << "Скорость собачки " << dog.second->GetVelocity().horizontal
//              << " " << dog.second->GetVelocity().vertical << std::endl;
//    std::cout << "Orientation " << dog.second->GetDirection() << std::endl;
//    std::cout << "Координаты собачки " << dog.second->GetPosition().x << " " << dog.second->GetPosition().y << std::endl;
//    std::cout << "Time delta " << time_delta << std::endl;

    std::pair<bool, Position> instructions_for_dog = map_.CalculateNewPosition(dog.second->GetPosition(), dog.second->GetVelocity(), time_delta);
//    std::cout << "Ставлю собаке " << new_position.x << " " << new_position.y << std::endl;
    dog.second->SetPosition(instructions_for_dog.second);

    if(instructions_for_dog.first) {
      dog.second->Stop();
    }

    //    std::cout << "____________________" << std::endl;
  }
}

} // model
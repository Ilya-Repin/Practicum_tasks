#include "Application.h"

namespace app {

std::pair<Token, model::Dog::Id> Application::JoinGame(std::string dog_name, model::Map::Id map_id) {
  if (!game_.FindMap(map_id)) {
    throw std::invalid_argument("Map not found");
  }

  std::shared_ptr<model::GameSession> game_session;

  for (auto &gs : sessions_) {
    if (gs->GetMapId() == map_id && gs->CanAddPlayer()) {
      game_session = gs;
    }
  }

  if (!game_session) {
    game_session = AddGameSession(*game_.FindMap(map_id));
  }

  auto dog_ptr = game_session->CreateDog(dog_name);
  return players_.AddPlayer(dog_ptr, game_session);
}

std::shared_ptr<model::GameSession> Application::AddGameSession(const model::Map &map) {
  using model::GameSession;

  std::shared_ptr<GameSession> session_ptr = std::make_shared<GameSession>(map, GameSession::Id(sessions_counter));
  sessions_.push_back(session_ptr);
  session_id_to_session.insert({model::GameSession::Id(sessions_counter), session_ptr});

  ++sessions_counter;

  return sessions_.back();
}
std::vector<std::pair<uint64_t, std::string>> Application::ListPlayers(Token token) {
  if (!players_.GetByToken(token)) {
    return {};
  }

  return session_id_to_session[(players_.GetByToken(token)->GetGameSessionId())]->GetPlayersData();
}

std::vector<std::shared_ptr<model::Map>> Application::ListMaps() {
  return game_.GetMaps();
}

const std::shared_ptr<model::Map> Application::GetMap(model::Map::Id id) {
  auto ptr = game_.FindMap(id);

  if (!ptr) {
    return nullptr;
  }

  return ptr;
}

bool Application::IsTokenExist(Token token) {
  return players_.IsTokenExist(token);
}

std::vector<GameState> Application::GetGameState(Token token) {
  if (!players_.GetByToken(token)) {
    return {};
  }

  auto session = session_id_to_session[(players_.GetByToken(token)->GetGameSessionId())];
  auto dogs = session->GetDogs();

  std::vector<GameState> game_state;

  std::unordered_map<model::Direction, std::string> dir_to_char
      {{model::Direction::NORTH, "U"}, {model::Direction::SOUTH, "D"},
       {model::Direction::EAST, "R"}, {model::Direction::WEST, "L"}};

  for (auto& dog : dogs) {
    auto dog_ptr = dog.second;
    game_state.push_back({dog.first, dog_ptr->GetPosition(), dog_ptr->GetVelocity(), dir_to_char[dog_ptr->GetDirection()]});
  }

  return game_state;
}

//bool Application::DoAction(Token token, std::string dir) {
//  if (!players_.GetByToken(token)) {
//    return false;
//  }
//
//  //players_.GetByToken(token)->SetDirection(dir.at(0));
//  return true;
//}

} // app
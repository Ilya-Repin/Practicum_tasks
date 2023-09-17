#pragma once

#include "../model/model.h"
#include "Player.h"
#include "../model/game_session.h"

namespace app {

class Application {
 public:
  Application(model::Game &game) :game_(game) {
  }

  // Use-cases
  std::pair<Token, model::Dog::Id> JoinGame(std::string name, model::Map::Id map_id);
  std::vector<std::pair<uint64_t, std::string>> ListPlayers(Token token); // <Dog::Id, dog_name>
  std::vector<model::Map> ListMaps();
  const model::Map* GetMap(model::Map::Id id);

  // Methods
  bool IsTokenExist(Token token);

 private:
  model::Game& game_;
  Players players_;
  std::vector<std::shared_ptr<model::GameSession>> sessions_; // TODO Когда заработает, попробовать перенести это в Game
  std::unordered_map<model::GameSession::Id, std::shared_ptr<model::GameSession>, model::GameSession::SessionIdHasher> session_id_to_session;
  uint64_t sessions_counter = 0;
  uint64_t dog_counter = 0;

  // Methods
  std::shared_ptr<model::GameSession> AddGameSession(const model::Map &map);
};

} // app
#pragma once

#include "../model/model.h"
#include "Player.h"

namespace app {

struct GameState {
  model::Dog::Id id;
  model::Position pos;
  model::Velocity speed;
  std::string dir;
  model::Dog::Bag bag;
  uint64_t score;
};

class Application {
 public:
  Application(model::Game &game, bool is_spawn_point_random)
      : game_(game), is_spawn_point_random_(is_spawn_point_random) {
  }

  // Use-cases
  std::pair<Token, model::Dog::Id> JoinGame(std::string name, model::Map::Id map_id);
  std::vector<std::pair<uint64_t, std::string>> ListPlayers(Token token); // <Dog::Id, dog_name>
  std::vector<std::shared_ptr<model::Map>> ListMaps();
  std::shared_ptr<model::Map> GetMap(model::Map::Id id) const;
  std::pair<std::vector<GameState>, std::unordered_map<uint64_t, model::Loot>> GetGameState(Token token) const;
  bool DoAction(Token token, std::string dir);
  void Tick(uint64_t time_delta);

  // Methods
  bool IsTokenExist(Token token);

 private:
  model::Game &game_;
  Players players_;
  uint64_t sessions_counter = 0;
  bool is_spawn_point_random_;

  std::vector<std::shared_ptr<model::GameSession>> sessions_;
  std::unordered_map<model::GameSession::Id, std::shared_ptr<model::GameSession>, model::GameSession::SessionIdHasher>
      session_id_to_session;

  // Methods
  std::shared_ptr<model::GameSession> AddGameSession(const model::Map &map);
};

} // namespace app
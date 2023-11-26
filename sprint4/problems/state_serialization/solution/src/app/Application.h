#pragma once

#include "../model/model.h"
#include "Player.h"
#include <memory>
#include <utility>

namespace app {

struct GameState {
  model::Dog::Id id;
  model::Position pos;
  model::Velocity speed;
  std::string dir;
  model::Dog::Bag bag;
  uint64_t score;
};

class ApplicationListener;

class Application : public std::enable_shared_from_this<Application> {
 public:
  using Sessions = std::vector<std::shared_ptr<model::GameSession>>;

  Application(model::Game &game, bool is_spawn_point_random, std::shared_ptr<ApplicationListener> listener)
      : game_(game), is_spawn_point_random_(is_spawn_point_random), listener_(listener) {

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

  //Only for serialization
  void InitSessions(Sessions &sessions);
  void InitPlayers(std::vector<std::shared_ptr<Player>> players);

  const Players& GetPlayers() const;
  std::vector<std::shared_ptr<model::GameSession>> GetGameSessions() const;
  std::shared_ptr<model::GameSession> GetGameSessionById(const model::GameSession::Id &id) const;
  bool IsSpawnPointRandom() const;
  model::LootGeneratorConfig GetLootConfig() const;

 private:
  std::shared_ptr<model::GameSession> AddGameSession(const model::Map &map);

 private:
  model::Game &game_;
  bool is_spawn_point_random_;

  Players players_;
  Sessions sessions_;
  uint64_t sessions_counter_ = 0;

  std::unordered_map<model::GameSession::Id, std::shared_ptr<model::GameSession>, model::GameSession::SessionIdHasher>
      session_id_to_session;

  std::shared_ptr<ApplicationListener> listener_;
};

} // namespace app
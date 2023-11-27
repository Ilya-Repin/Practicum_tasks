#pragma once

#include "../model/model.h"
#include "Player.h"
#include "../database/postgres.h"
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

  Application(model::Game &game, bool is_spawn_point_random,
              std::shared_ptr<ApplicationListener> listener,
              std::pair<std::size_t, std::string> db_config)
      : game_(game),
        is_spawn_point_random_(is_spawn_point_random),
        listener_(std::move(listener)),
        db_{db_config.first, db_config.second} {
  }

  // Use-cases
  std::pair<Token, model::Dog::Id> JoinGame(std::string name, model::Map::Id map_id);
  std::vector<std::pair<uint64_t, std::string>> ListPlayers(Token token); // <Dog::Id, dog_name>
  std::vector<std::shared_ptr<model::Map>> ListMaps();
  std::shared_ptr<model::Map> GetMap(model::Map::Id id) const;
  std::pair<std::vector<GameState>, std::unordered_map<uint64_t, model::Loot>> GetGameState(Token token) const;
  bool DoAction(Token token, std::string dir);
  void Tick(uint64_t time_delta);
  std::vector<Retired> GetRecords(int start, int maxItems);

  // Predicates
  bool IsTokenExist(Token token);
  bool IsSpawnPointRandom() const;

  // Getters
  const model::Game &GetGame() const;
  const Players &GetPlayers() const;
  std::vector<std::shared_ptr<model::GameSession>> GetGameSessions() const;
  std::shared_ptr<model::GameSession> GetGameSessionById(const model::GameSession::Id &id) const;
  model::LootGeneratorConfig GetLootConfig() const;

  // Methods
  bool InitSessions(Sessions &sessions);
  bool InitPlayers(std::unordered_map<Token, std::shared_ptr<Player>, Players::TokenIdHasher> &&players);

 private:
  std::shared_ptr<model::GameSession> AddGameSession(const model::Map &map);

 private:
  model::Game &game_;
  Players players_;
  Sessions sessions_;
  postgres::Database db_;

  bool is_spawn_point_random_;
  uint64_t sessions_counter_ = 0;

  std::unordered_map<model::GameSession::Id, std::shared_ptr<model::GameSession>, model::GameSession::SessionIdHasher>
      session_id_to_session;

  // For serialization
  std::shared_ptr<ApplicationListener> listener_;
  bool is_sessions_init = false;
  bool is_players_init = false;
};

} // namespace app
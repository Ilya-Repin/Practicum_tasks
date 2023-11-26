#include "Application.h"
#include "ApplicationListener.h"

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

std::vector<std::pair<uint64_t, std::string>> Application::ListPlayers(Token token) {
  if (!players_.GetByToken(token)) {
    return {};
  }

  return session_id_to_session[(players_.GetByToken(token)->GetGameSessionId())]->GetPlayersData();
}

std::vector<std::shared_ptr<model::Map>> Application::ListMaps() {
  return game_.GetMaps();
}

std::shared_ptr<model::Map> Application::GetMap(model::Map::Id id) const {
  auto ptr = game_.FindMap(id);

  if (!ptr) {
    return nullptr;
  }

  return ptr;
}

std::pair<std::vector<GameState>,
          std::unordered_map<uint64_t, model::Loot>> Application::GetGameState(Token token) const {
  if (!players_.GetByToken(token)) {
    return {};
  }

  auto session = session_id_to_session.at(players_.GetByToken(token)->GetGameSessionId());
  auto dogs = session->GetDogs();

  std::vector<GameState> game_state;

  std::unordered_map<model::Direction, std::string> dir_to_char
      {{model::Direction::NORTH, "U"}, {model::Direction::SOUTH, "D"},
       {model::Direction::EAST, "R"}, {model::Direction::WEST, "L"}};

  for (auto &dog : dogs) {
    auto dog_ptr = dog.second;
    game_state.push_back({dog.first, dog_ptr->GetPosition(),
                          dog_ptr->GetVelocity(),
                          dir_to_char[dog_ptr->GetDirection()],
                          dog_ptr->GetBag(),
                          dog_ptr->GetScore()});
  }

  return {game_state, session->GetLoots()};
}

bool Application::DoAction(Token token, std::string dir) {
  if (!players_.GetByToken(token)) {
    return false;
  }

  auto dog = players_.GetByToken(token)->GetDog();
  dog->SetDirection(*dir.data());

  return true;
}

void Application::Tick(uint64_t time_delta) {
  if (listener_) {
    listener_->OnTick(std::chrono::milliseconds(time_delta), shared_from_this());
  }

  std::vector<std::pair<model::GameSession::Id, std::shared_ptr<model::Dog>>> retired;

  for (auto &session : sessions_) {
    auto retired_dogs = session->Tick(time_delta);

    for (auto dog : retired_dogs) {
      retired.emplace_back(session->GetId(), dog);
    }
  }

  for (auto &player : retired) {
    players_.RetirePlayer({player.first, player.second->GetId()});
    Retired champion{RetiredId::New(),
                     player.second->GetName(),
                     player.second->GetScore(),
                     player.second->GetGamingTime()};
    db_.GetRetiredDogs().Save(champion);
  }
}

std::vector<Retired> Application::GetRecords(int start, int max_items) {
  return db_.GetRetiredDogs().GetRetiredDogs(start, max_items);
}

bool Application::IsTokenExist(Token token) {
  return players_.IsTokenExist(token);
}

std::shared_ptr<model::GameSession> Application::AddGameSession(const model::Map &map) {
  using model::GameSession;

  std::shared_ptr<GameSession> session_ptr = std::make_shared<GameSession>(map,
                                                                           GameSession::Id(sessions_counter_),
                                                                           is_spawn_point_random_,
                                                                           game_.GetLootGeneratorConfig(),
                                                                           game_.GetDogRetirementTime());

  sessions_.push_back(session_ptr);
  session_id_to_session.insert({model::GameSession::Id(sessions_counter_), session_ptr});

  ++sessions_counter_;

  return sessions_.back();
}

bool Application::InitSessions(Application::Sessions &sessions) {
  if (is_sessions_init) {
    return false;
  }

  sessions_ = sessions;

  session_id_to_session.clear();
  sessions_counter_ = 0;

  for (auto &session : sessions_) {
    session_id_to_session.insert({model::GameSession::Id(sessions_counter_), session});
    ++sessions_counter_;
  }

  is_sessions_init = true;

  return is_sessions_init;
}

bool Application::InitPlayers(std::unordered_map<Token, std::shared_ptr<Player>, Players::TokenIdHasher> &&players) {
  if (is_players_init) {
    return false;
  }

  players_.Init(std::move(players));

  is_players_init = true;

  return is_players_init;
}

const Players &Application::GetPlayers() const {
  return players_;
}

std::vector<std::shared_ptr<model::GameSession>> Application::GetGameSessions() const {
  return sessions_;
}

std::shared_ptr<model::GameSession> Application::GetGameSessionById(const model::GameSession::Id &id) const {
  return session_id_to_session.at(id);
}

bool Application::IsSpawnPointRandom() const {
  return is_spawn_point_random_;
}

model::LootGeneratorConfig Application::GetLootConfig() const {
  return game_.GetLootGeneratorConfig();
}

const model::Game &Application::GetGame() const {
  return game_;
}

} // namespace app
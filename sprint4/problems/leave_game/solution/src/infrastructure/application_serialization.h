#pragma once
#include <boost/serialization/vector.hpp>
#include "../app/Application.h"
#include "model_serialization.h"

namespace serialization {

class PlayerRepr {
 public:
  PlayerRepr() = default;

  explicit PlayerRepr(std::shared_ptr<app::Player> player) : dog_id_(player->GetDogId()),
                                                             session_id_(player->GetGameSessionId()),
                                                             token_(player->GetToken()) {
  }

  [[nodiscard]] std::shared_ptr<app::Player> Restore(std::shared_ptr<app::Application> app) const {
    auto session = app->GetGameSessionById(session_id_);
    auto dog = session->GetDogs().at(dog_id_);

    return std::make_shared<app::Player>(session, dog, token_);
  }

  template<typename Archive>
  void serialize(Archive &ar, [[maybe_unused]] const unsigned version) {
    ar & *dog_id_;
    ar & *session_id_;
    ar & *token_;
  }

 private:
  model::Dog::Id dog_id_{0};
  model::GameSession::Id session_id_{0};
  app::Token token_{""};
};

class PlayersRepr {
 public:
  PlayersRepr() = default;

  explicit PlayersRepr(const app::Players &players) {
    for (auto player : players.GetPlayers()) {
      players_.emplace_back(player.second);
    }
  }

  std::unordered_map<app::Token,
                     std::shared_ptr<app::Player>,
                     app::Players::TokenIdHasher> Restore(std::shared_ptr<app::Application> app) const {
    std::unordered_map<app::Token, std::shared_ptr<app::Player>, app::Players::TokenIdHasher> token_to_player;

    for (auto &player : players_) {
      std::shared_ptr<app::Player> player_ptr(player.Restore(app));
      token_to_player.insert({player_ptr->GetToken(), player_ptr});
    }

    return token_to_player;
  }

  template<typename Archive>
  void serialize(Archive &ar, [[maybe_unused]] const unsigned version) {
    ar & players_;
  }

 private:
  std::vector<PlayerRepr> players_;
};

class AppRepr {
 public:
  AppRepr() = default;

  explicit AppRepr(std::shared_ptr<app::Application> app) : players_(app->GetPlayers()) {
    for (auto &session : app->GetGameSessions()) {
      sessions_.emplace_back(*session);
    }
  }

  void Restore(std::shared_ptr<app::Application> app) const {
    app::Application::Sessions sessions;

    for (auto &session : sessions_) {
      sessions.emplace_back(std::make_shared<model::GameSession>(session.Restore(app)));
    }

    bool is_sessions_upload = app->InitSessions(sessions);
    bool is_players_upload = app->InitPlayers(players_.Restore(app));

    if (!is_sessions_upload || !is_players_upload) {
      throw std::runtime_error("App is already init");
    }
  }

  template<typename Archive>
  void serialize(Archive &ar, [[maybe_unused]] const unsigned version) {
    ar & players_;
    ar & sessions_;
  }

 private:
  PlayersRepr players_;
  std::vector<GameSessionRepr> sessions_;
};

} // namespace serialization
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
    std::cout << "Restore pl 1" << *session_id_ << std::endl;

    auto session = app->GetGameSessionById(session_id_);
    std::cout << "Restore pl 2" << std::endl;

    auto dog = session->GetDogs().at(dog_id_);
    std::cout << "Restore pl 3" << std::endl;

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

  explicit PlayersRepr(const app::Players& players) {
    //std::cout << "I'm here!" << std::endl;

    //std::cout << "I'm here 1!"  <<std::endl;

    for (auto player : players.GetPlayers()) {
      //std::cout << "I'm in cycle!" << std::endl;

      players_.emplace_back(player);
    }
    //std::cout << "I'm there 1!" << std::endl;

  }

  std::vector<std::shared_ptr<app::Player>> Restore(std::shared_ptr<app::Application> app) const {
    std::vector<std::shared_ptr<app::Player>> players;
    std::cout << "Restore 1" << std::endl;

    for (auto player : players_) {
      std::cout << "Restore 1.25" << std::endl;
      players.push_back(player.Restore(app));
      std::cout << "Restore 1.75" << std::endl;

    }
    std::cout << "Restore 2" << std::endl;

    return players;
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
    //std::cout << "AppRepr start creating 1" << std::endl;

    for (auto &session : app->GetGameSessions()) {
      sessions_.emplace_back(*session);
      //std::cout << "AppRepr start creating 2" << std::endl;
    }
  }

  void Restore(std::shared_ptr<app::Application> app) const {
    app::Application::Sessions sessions;
    std::cout << "AppRepr Restore 1" << std::endl;

    for (auto &session : sessions_) {
      sessions.emplace_back(std::make_shared<model::GameSession>(session.Restore(app)));
    }

    std::cout << "AppRepr Restore 2, sessions amount" <<  sessions.size() << std::endl;

    app->InitSessions(sessions);
    app->InitPlayers(players_.Restore(app));
    std::cout << "AppRepr Restore 3" << std::endl;

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
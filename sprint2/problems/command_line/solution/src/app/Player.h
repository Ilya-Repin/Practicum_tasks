#pragma once
#include <random>

#include "../model/game_session.h"

namespace app {

namespace detail {
struct TokenTag {};
}  // namespace detail

using Token = util::Tagged<std::string, detail::TokenTag>;

class Player {
 public:
  using Id = std::pair<model::GameSession::Id, model::Dog::Id>;

  Player(std::shared_ptr<model::GameSession> gs, std::shared_ptr<model::Dog> dog, Token token)
      : session_(std::move(gs)), dog_(std::move(dog)), token_(std::move(token)), id_(session_->GetId(), dog_->GetId()) {
  }

  // Getters
  Token GetToken() const noexcept;
  const Player::Id GetId() const noexcept;
  const model::Dog::Id GetDogId() const noexcept;
  const model::GameSession::Id &GetGameSessionId() const noexcept;
  const model::Map::Id GetMapId() const noexcept;
  const std::shared_ptr<model::Dog> GetDog() const noexcept;

 private:
  std::shared_ptr<model::GameSession> session_;
  std::shared_ptr<model::Dog> dog_;
  Id id_;
  Token token_;
};

struct TokenGenerator {
 public:
  Token GenerateNewToken();

 private:
  std::random_device random_device_;
  std::mt19937_64 generator1_{[this] {
    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
    return dist(random_device_);
  }()};

  std::mt19937_64 generator2_{[this] {
    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
    return dist(random_device_);
  }()};
};

class Players {
 public:
  // Add
  std::pair<Token, model::Dog::Id> AddPlayer(std::shared_ptr<model::Dog> &dog, std::shared_ptr<model::GameSession> gs);

  // Getters
  std::shared_ptr<Player> GetByDogIdAndSessionId(model::Dog::Id dog_id, model::GameSession::Id gs_id) const;
  std::shared_ptr<Player> GetByToken(Token &token) const;
  size_t GetPlayersAmount() const;

  // Methods
  bool IsTokenExist(Token token);

 private:
  using TokenIdHasher = util::TaggedHasher<Token>;

  struct PlayerIdHash {
    std::size_t operator()(const std::pair<model::GameSession::Id, model::Dog::Id> &p) const {
      std::size_t uint64_hash_1 = std::hash<uint64_t>{}(*p.first);
      std::size_t uint64_hash_2 = std::hash<uint64_t>{}(*p.second);
      return uint64_hash_1 ^ (uint64_hash_2 << 1);
    }
  };

  std::vector<std::shared_ptr<Player>> players_;

  std::unordered_map<Token, size_t, TokenIdHasher> token_to_player_index_;
  std::unordered_map<Player::Id, size_t, PlayerIdHash> player_id_to_player_index_;

  TokenGenerator token_generator_;
};

} // app
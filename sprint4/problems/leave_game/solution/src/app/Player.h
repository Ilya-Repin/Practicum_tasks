#pragma once
#include <random>

#include "../model/game_session.h"
#include "../util/tagged_uuid.h"

namespace app {

namespace detail {
struct TokenTag {};
struct ChampionTag {};
}  // namespace detail

using Token = util::Tagged<std::string, detail::TokenTag>;
using RetiredId = util::TaggedUUID<detail::ChampionTag>;

struct Retired {
  RetiredId id;
  std::string name;
  uint64_t score;
  uint64_t play_time_ms;
};

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
  using TokenIdHasher = util::TaggedHasher<Token>;

  // Constructors
  Players() = default;

  // Getters
  std::shared_ptr<Player> GetByDogIdAndSessionId(model::Dog::Id dog_id, model::GameSession::Id gs_id) const;
  std::shared_ptr<Player> GetByToken(Token &token) const;
  size_t GetPlayersAmount() const;
  std::unordered_map<Token, std::shared_ptr<Player>, TokenIdHasher> GetPlayers() const;

  // Methods
  bool IsTokenExist(Token token);
  std::pair<Token, model::Dog::Id> AddPlayer(std::shared_ptr<model::Dog> &dog, std::shared_ptr<model::GameSession> gs);
  void RetirePlayer(Player::Id id);

 private:
  void Init(std::unordered_map<Token, std::shared_ptr<Player>, TokenIdHasher> &&token_to_player);

 private:
  struct PlayerIdHash {
    std::size_t operator()(const std::pair<model::GameSession::Id, model::Dog::Id> &p) const {
      return std::hash<uint64_t>{}(*p.first) ^ (std::hash<uint64_t>{}(*p.second) << 1);
    }
  };

  std::unordered_map<Token, std::shared_ptr<Player>, TokenIdHasher> token_to_player_;
  std::unordered_map<Player::Id, std::shared_ptr<Player>, PlayerIdHash> player_id_to_player_;

  TokenGenerator token_generator_;

  friend class Application;
};

} // namespace app
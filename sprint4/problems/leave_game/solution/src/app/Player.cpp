#include "Player.h"
#include <sstream>
#include <iomanip>

namespace app {

std::shared_ptr<Player> Players::GetByDogIdAndSessionId(model::Dog::Id dog_id, model::GameSession::Id gs_id) const {
  return player_id_to_player_.at(std::make_pair(gs_id, dog_id));
}

std::shared_ptr<Player> Players::GetByToken(Token &token) const {
  return token_to_player_.at(token);
}

size_t Players::GetPlayersAmount() const {
  return token_to_player_.size();
}

Token Player::GetToken() const noexcept {
  return token_;
}

const Player::Id Player::GetId() const noexcept {
  return id_;
}

const model::GameSession::Id &Player::GetGameSessionId() const noexcept{
  return session_->GetId();
}

const model::Map::Id Player::GetMapId() const noexcept {
  return session_->GetMapId();
}

const model::Dog::Id Player::GetDogId() const noexcept {
  return dog_->GetId();
}

const std::shared_ptr<model::Dog> Player::GetDog() const noexcept {
  return dog_;
}

std::unordered_map<Token, std::shared_ptr<Player>, Players::TokenIdHasher> Players::GetPlayers() const {
  return token_to_player_;
}

std::pair<Token, model::Dog::Id> Players::AddPlayer(std::shared_ptr<model::Dog> &dog,
                                                    std::shared_ptr<model::GameSession> gs) {
  Token token = token_generator_.GenerateNewToken();

  auto new_player = std::make_shared<Player>(gs, dog, token);

  token_to_player_.emplace(token, new_player);
  player_id_to_player_.emplace(Player::Id(gs->GetId(), dog->GetId()), new_player);

  return {token, dog->GetId()};
}

bool Players::IsTokenExist(Token token) {
  return token_to_player_.contains(token);
}

void Players::Init(std::unordered_map<Token, std::shared_ptr<Player>, TokenIdHasher>&& token_to_player) {
  token_to_player_ = std::move(token_to_player);

  for(auto &player : token_to_player_) {
      player_id_to_player_.emplace(player.second->GetId(), player.second);
  }
}

void Players::RetirePlayer(Player::Id id) {
  token_to_player_.erase(player_id_to_player_.at(id)->GetToken());
  player_id_to_player_.erase(id);
}

Token TokenGenerator::GenerateNewToken() {
  std::stringstream ss;

  while (ss.str().size() != 32) {
    ss.clear();
    ss << std::setfill('0') << std::setw(16) << std::hex << generator1_();
    ss << std::setfill('0') << std::setw(16) << std::hex << generator2_();
  }

  return Token(ss.str());
}

} // namespace app
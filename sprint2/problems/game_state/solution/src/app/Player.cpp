#include "Player.h"
#include <sstream>
#include <iomanip>

namespace app {

std::shared_ptr<Player> Players::GetByDogIdAndSessionId(model::Dog::Id dog_id, model::GameSession::Id gs_id) {
  return players_[player_id_to_player_index_.at(std::make_pair(gs_id, dog_id))];
}

std::shared_ptr<Player> Players::GetByToken(Token &token) {
  return players_[token_to_player_index_.at(token)];
}

size_t Players::GetPlayersAmount() {
  return players_.size();
}

std::pair<Token, model::Dog::Id> Players::AddPlayer(std::shared_ptr<model::Dog> &dog,
                                                    std::shared_ptr<model::GameSession> gs) {
  Token token = token_generator_.GenerateNewToken();

  players_.push_back(std::make_shared<Player>(gs, dog, token));

  token_to_player_index_.emplace(token, players_.size() - 1);
  player_id_to_player_index_.emplace(Player::Id(gs->GetId(), dog->GetId()), players_.size() - 1);

  return {token, dog->GetId()};
}

bool Players::IsTokenExist(Token token) {
  return token_to_player_index_.contains(token);
}

Token Player::GetToken() const noexcept {
  return token_;
}

const Player::Id Player::GetId() const noexcept {
  return id_;
}

const model::GameSession::Id Player::GetGameSessionId() const noexcept {
  return session_->GetId();
}

const model::Map::Id Player::GetMapId() const noexcept {
  return session_->GetMapId();
}

const model::Dog::Id Player::GetDogId() const noexcept {
  return dog_->GetId();
}

Token TokenGenerator::GenerateNewToken() {
  std::stringstream ss;

  while(ss.str().size() != 32) {
    ss.clear();
    ss << std::setfill('0') << std::setw(16) << std::hex << generator1_();
    ss << std::setfill('0') << std::setw(16) << std::hex << generator2_();
  }

  return Token(ss.str());
}


} // app
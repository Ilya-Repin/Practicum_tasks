#include "dog.h"

namespace model {

uint64_t Dog::dog_counter_ = 0;

double Distance(const Position &p1, const Position &p2) {
  return std::abs(((p1 - p2).x != 0 ? (p1 - p2).x : (p1 - p2).y));
}

// Operator's overloading
Position operator-(const Position &p1, const Position &p2) {
  Position result;
  result.x = p1.x - p2.x;
  result.y = p1.y - p2.y;

  return result;
}

Position Position::operator+(const Position &other) {
  Position result;

  result.x = this->x + other.x;
  result.y = this->y + other.y;

  return result;
}

bool Position::operator==(const Position &other) {
  return (this->x == other.x) && (this->y == other.y);
}

bool Position::operator!=(const Position &other) {
  return (this->x != other.x) || (this->y != other.y);
}

bool Velocity::operator==(const Velocity &other) const {
  return this->vertical == other.vertical && this->horizontal == other.horizontal;
}

// Getters
const std::string &Dog::GetName() const noexcept {
  return name_;
}

Direction Dog::GetDirection() const noexcept {
  return dir_;
}

Velocity Dog::GetVelocity() const noexcept {
  return speed_;
}

Position Dog::GetPosition() const noexcept {
  return pos_;
}

Position Dog::GetPreviousPosition() const noexcept {
  return previous_position_;
}

const Dog::Bag &Dog::GetBag() const noexcept {
  return bag_;
}

const Dog::Id &Dog::GetId() const noexcept {
  return id_;
}

// Setters
void Dog::SetDirection(char direction) {
  switch (direction) {
    case 'L':dir_ = Direction::WEST;
      speed_ = {-default_speed_, 0};
      break;

    case 'R':dir_ = Direction::EAST;
      speed_ = {default_speed_, 0};
      break;

    case 'U':dir_ = Direction::NORTH;
      speed_ = {0, -default_speed_};
      break;

    case 'D':dir_ = Direction::SOUTH;
      speed_ = {0, default_speed_};
      break;

    case '\0':speed_ = {0, 0};
      break;

    default:speed_ = {0, 0};
      break;
  }
}

void Dog::SetPosition(Position position) {
  previous_position_ = pos_;
  pos_ = position;
}

void Dog::Stop() {
  speed_ = {0, 0};
}

void Dog::TakeLoot(Thing loot) {
  if (bag_.size() <= bag_capacity_) {
    bag_.push_back(loot);
  }
}

void Dog::UnloadBag(const std::vector<uint64_t> &values) {
  for(auto thing : bag_) {
    score_ += values[thing.type];
  }

  bag_.clear();
}

bool Dog::IsBagFull() {
  return bag_.size() == bag_capacity_;
}

uint64_t Dog::GetScore() const noexcept {
  return score_;
}

double Dog::GetDefaultSpeed() const {
  return default_speed_;
}

void Dog::SetScore(uint64_t new_score) {
  score_ = new_score;
}

} // namespace model
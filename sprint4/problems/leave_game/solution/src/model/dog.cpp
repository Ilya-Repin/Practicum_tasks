#include <iostream>
#include "dog.h"

namespace model {

uint64_t Dog::dog_counter_ = 0;

double Distance(const Position &p1, const Position &p2) {
  return std::abs(((p1 - p2).x != 0 ? (p1 - p2).x : (p1 - p2).y));
}

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

double Dog::GetDefaultSpeed() const {
  return default_speed_;
}

size_t Dog::GetBagCapacity() const noexcept {
  return bag_capacity_;
}

uint64_t Dog::GetGamingTime() const {
  return gaming_time_;
}

uint64_t Dog::GetIdleTime() const {
  return idle_time_;
}

uint64_t Dog::GetDogCounter() const {
  return dog_counter_;
}

uint64_t Dog::GetScore() const noexcept {
  return score_;
}

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

  if (speed_.horizontal != 0 || speed_.vertical != 0) {
    idle_time_ = 0;
  }
}

void Dog::SetScore(uint64_t new_score) {
  score_ = new_score;
}

void Dog::SetPosition(Position position) {
  previous_position_ = pos_;
  pos_ = position;
}

void Dog::Stop() {
  speed_ = {0, 0};
}

void Dog::TakeLoot(Thing loot) {
  if (bag_.size() < bag_capacity_) {
    bag_.push_back(loot);
  }
}

void Dog::UnloadBag(const std::vector<uint64_t> &values) {
  for (auto thing : bag_) {
    score_ += values[thing.type];
  }

  bag_.clear();
}

void Dog::UpdateTime(uint64_t time_delta) {
  gaming_time_ += time_delta;

  if (IsStopped()) {
    idle_time_ += time_delta;
  } else {
    idle_time_ = 0;
  }
}

bool Dog::Init(uint64_t dog_counter, uint64_t gaming_time) {
  if (is_init_) {
    return false;
  }

  dog_counter_ = dog_counter;
  gaming_time_ = gaming_time;

  is_init_ = true;

  return is_init_;
}

bool Dog::IsStopped() const {
  return (speed_.horizontal == 0.0 && speed_.vertical == 0.0);
}

bool Dog::IsBagFull() const {
  return bag_.size() == bag_capacity_;
}

} // namespace model
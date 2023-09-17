#pragma once

#include <string>
#include "../tagged.h"
#include "model.h"

namespace model {

struct Position {
  double x = 0.0;
  double y = 0.0;
};

struct Velocity {
  double vertical = 0.0;
  double horizontal = 0.0;
};

enum Direction {
  NORTH, WEST, EAST, SOUTH
};

class Dog {
 public:
  using Id = util::Tagged<uint64_t, Dog>;

  Dog(std::string name) noexcept //, double speed
      : id_(++dog_counter_), name_(std::move(name)){//, default_speed(speed) {
  }

  const Id &GetId() const noexcept {
    return id_;
  }

  Position GetPosition() const noexcept {
    return pos_;
  }

  Velocity GetVelocity() {
    return speed_;
  }

  Direction GetDirection() {
    return dir_;
  }

  const std::string &GetName() {
    return name_;
  }

  /*// Setter
  void SetDirection(char direction) {
    switch (direction) {
      case 'L':
        dir_ = Direction::WEST;
        speed_ = {-default_speed, 0};
        break;

      case 'R':
        dir_ = Direction::WEST;
        speed_ = {-default_speed, 0};
        break;

      case 'U':
        dir_ = Direction::NORTH;
        speed_ = {0, -default_speed};
        break;

      case 'D':
        dir_ = Direction::SOUTH;
        speed_ = {0, default_speed};
        break;

      case '\0': //TODO Здесь может быть проблема
        speed_ = {0, 0};
        break;
      default:
        break;
    }
  }*/

 private:
  Id id_;
  std::string name_;
  Position pos_{0, 0};
  Velocity speed_;
  Direction dir_ = Direction::NORTH;

  //double default_speed;
  static uint64_t dog_counter_; // TODO может атомик? И как это инициализировать?
};

} // model
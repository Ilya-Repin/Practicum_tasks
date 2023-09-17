#pragma once

#include <string>
#include "../tagged.h"
#include <limits>

namespace model {

struct Position {
  double x = 0.0;
  double y = 0.0;

  Position operator+(const Position& other);
  bool operator==(const Position& other);
  bool operator!=(const Position& other);
};

Position operator-(const Position& p1, const Position& p2);
double Distance (const Position& p1, const Position& p2);

struct Velocity {
  double horizontal = 0.0;
  double vertical = 0.0;

  bool operator==(const Velocity& other) const;
};


enum Direction {
  NORTH, WEST, EAST, SOUTH
};

class Dog {
 public:
  using Id = util::Tagged<uint64_t, Dog>;

  Dog(std::string name, double speed, Position start_point) noexcept
      : id_(++dog_counter_), name_(std::move(name)), default_speed_(speed), pos_(start_point) {
  }


  const Id &GetId() const noexcept;

  Position GetPosition() const noexcept;

  Velocity GetVelocity();

  Direction GetDirection();

  const std::string &GetName();

  // Setter
  void SetDirection(char direction);

  void SetPosition(Position position);

  void Stop();

 private:
  Id id_;
  std::string name_;
  Position pos_; // TODO Должно быть базово рандом
  Velocity speed_;
  Direction dir_ = Direction::NORTH;

  double default_speed_;
  static uint64_t dog_counter_; // TODO может атомик?
};

} // model

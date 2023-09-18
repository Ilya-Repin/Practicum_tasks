#pragma once

#include <string>
#include <limits>

#include "../util/tagged.h"

namespace model {

struct Position {
  double x = 0.0;
  double y = 0.0;

  Position operator+(const Position &other);

  bool operator==(const Position &other);
  bool operator!=(const Position &other);
};

Position operator-(const Position &p1, const Position &p2);

double Distance(const Position &p1, const Position &p2);

struct Velocity {
  double horizontal = 0.0;
  double vertical = 0.0;

  bool operator==(const Velocity &other) const;
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

  // Getters
  const Id &GetId() const noexcept;
  Position GetPosition() const noexcept;
  Velocity GetVelocity() const noexcept;
  Direction GetDirection() const noexcept;
  const std::string &GetName() const noexcept;

  // Setters
  void SetDirection(char direction);
  void SetPosition(Position position);
  void Stop();

 private:
  Id id_;
  std::string name_;
  Position pos_;
  Velocity speed_;
  Direction dir_ = Direction::NORTH;

  double default_speed_;
  static uint64_t dog_counter_;
};

} // model

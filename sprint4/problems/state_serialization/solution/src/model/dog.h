#pragma once

#include <string>
#include <limits>
#include <vector>

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

struct Thing {
  uint64_t id;
  int type;
};

enum Direction {
  NORTH, WEST, EAST, SOUTH
};

class Dog {
 public:
  using Id = util::Tagged<uint64_t, Dog>;
  using Bag = std::vector<Thing>;

  Dog(std::string name, double speed, Position start_point, size_t bag_capacity, uint64_t id = 0u) noexcept
      : id_(id != 0 ? id : ++dog_counter_), name_(std::move(name)), default_speed_(speed), pos_(start_point), previous_position_(start_point), bag_capacity_(bag_capacity) {
  }

  // Getters
  const Id &GetId() const noexcept;
  Position GetPosition() const noexcept;
  Position GetPreviousPosition() const noexcept;
  Velocity GetVelocity() const noexcept;
  Direction GetDirection() const noexcept;
  const Bag &GetBag() const noexcept;
  const std::string &GetName() const noexcept;
  uint64_t GetScore() const noexcept;
  double GetDefaultSpeed() const;

  // Setters
  void SetDirection(char direction);
  void SetPosition(Position position);
  void SetScore(uint64_t new_score);
  void Stop();

  // Methods
  void TakeLoot(Thing loot);
  void UnloadBag(const std::vector<uint64_t> &values);
  bool IsBagFull();

 private:
  Id id_;
  std::string name_;

  Position pos_;
  Position previous_position_;
  Velocity speed_;
  Direction dir_ = Direction::NORTH;

  Bag bag_;
  size_t bag_capacity_;
  uint64_t score_ = 0;

  double default_speed_;
  static uint64_t dog_counter_;
};

} // namespace model

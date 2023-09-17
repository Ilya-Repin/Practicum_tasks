#pragma once

#include <string>
#include "../tagged.h"
#include "model.h"

namespace model {

class Dog {
 public:
  using Id = util::Tagged<uint64_t, Dog>;

  Dog(Id id, std::string name) noexcept
      : id_(id), name_(std::move(name)) {
  }

  const Id &GetId() const noexcept {
    return id_;
  }

  Point GetPosition() const noexcept {
    return position_;
  }
  const std::string& GetName() {
    return name_;
  }
 private:
  Id id_;
  std::string name_;
  Point position_{0,0};
};

} // model

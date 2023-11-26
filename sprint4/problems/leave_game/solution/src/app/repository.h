#pragma once

#include <vector>
#include <string>
#include "Player.h"

namespace app {
class RetiredDogRepository {
 public:
  virtual void Save(const Retired &author) = 0;
  virtual std::vector<Retired> GetRetiredDogs(int start, int max_items) = 0;

 protected:
  ~RetiredDogRepository() = default;
};
} // namespace model
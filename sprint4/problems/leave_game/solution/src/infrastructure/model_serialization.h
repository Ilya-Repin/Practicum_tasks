#pragma once
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>

#include "../model/model.h"
#include "../model/game_session.h"
#include "../app/Application.h"

namespace model {

template<typename Archive>
void serialize(Archive &ar, model::Position &point, [[maybe_unused]] const unsigned version) {

  ar & point.x;
  ar & point.y;
}

template<typename Archive>
void serialize(Archive &ar, model::Velocity &speed, [[maybe_unused]] const unsigned version) {
  ar & speed.horizontal;
  ar & speed.vertical;
}

template<typename Archive>
void serialize(Archive &ar, model::Thing &obj, [[maybe_unused]] const unsigned version) {
  ar & (obj.id);
  ar & (obj.type);
}

template<typename Archive>
void serialize(Archive &ar, model::Loot &obj, [[maybe_unused]] const unsigned version) {
  ar & (obj.type);
  ar & (obj.value);
  ar & (obj.x);
  ar & (obj.y);
}

}  // namespace model

namespace serialization {

class DogRepr {
 public:
  DogRepr() = default;

  explicit DogRepr(const model::Dog &dog)
      : id_(dog.GetId()),
        name_(dog.GetName()),
        pos_(dog.GetPosition()),
        bag_(dog.GetBag()),
        bag_capacity_(dog.GetBagCapacity()),
        speed_(dog.GetVelocity()),
        direction_(dog.GetDirection()),
        score_(dog.GetScore()),
        default_speed_(dog.GetDefaultSpeed()),
        dog_counter_(dog.GetDogCounter()),
        gaming_time_(dog.GetGamingTime()) {
  }

  [[nodiscard]] model::Dog Restore() const {
    model::Dog dog{name_, default_speed_, pos_, bag_capacity_, *id_};
    dog.SetDirection(direction_);
    dog.SetScore(score_);
    dog.Init(dog_counter_, gaming_time_);

    for (const auto &item : bag_) {
      dog.TakeLoot({item.id, item.type});
    }

    return dog;
  }

  template<typename Archive>
  void serialize(Archive &ar, [[maybe_unused]] const unsigned version) {
    ar & *id_;
    ar & name_;
    ar & pos_;
    ar & bag_capacity_;
    ar & speed_;
    ar & default_speed_;
    ar & direction_;
    ar & score_;
    ar & bag_;
    ar & dog_counter_;
    ar & gaming_time_;
  }

 private:
  model::Dog::Id id_ = model::Dog::Id{0u};
  std::string name_;

  model::Position pos_;
  model::Velocity speed_;
  model::Direction direction_ = model::Direction::NORTH;

  model::Dog::Bag bag_;
  size_t bag_capacity_;
  uint64_t score_ = 0;

  double default_speed_;
  uint64_t dog_counter_;

  uint64_t gaming_time_;
};

class GameSessionRepr {
 public:
  GameSessionRepr() = default;

  explicit GameSessionRepr(const model::GameSession &session) : id_(session.GetId()), map_id_(session.GetMapId()) {
    for (auto &dog : session.GetDogs()) {
      dogs_.emplace_back(*dog.second);
    }

    for (auto &loot : session.GetLoots()) {
      loots_.insert(loot);
    }
  }

  [[nodiscard]] model::GameSession Restore(std::shared_ptr<app::Application> app) const {
    model::GameSession::Dogs dogs;
    model::GameSession::Loots loots;

    for (auto &dog_repr : dogs_) {
      auto dog = dog_repr.Restore();
      dogs.emplace(dog.GetId(), std::make_shared<model::Dog>(dog));
    }

    for (auto &loot : loots_) {
      loots.emplace(loot);
    }

    auto game_session = model::GameSession(*app->GetMap(map_id_),
                                           id_,
                                           app->IsSpawnPointRandom(),
                                           app->GetLootConfig(),
                                           app->GetGame().GetDogRetirementTime(),
                                           std::move(dogs),
                                           std::move(loots));

    return game_session;
  }

  template<typename Archive>
  void serialize(Archive &ar, [[maybe_unused]] const unsigned version) {
    ar & *id_;
    ar & *map_id_;
    ar & dogs_;
    ar & loots_;
  }

 private:
  model::GameSession::Id id_{0};
  model::Map::Id map_id_{""};
  std::vector<DogRepr> dogs_;
  std::unordered_map<uint64_t, model::Loot> loots_;
};

}  // namespace serialization

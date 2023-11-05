#define _USE_MATH_DEFINES
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iostream>

#include "../src/collision_detector.h"

class Provider : public collision_detector::ItemGathererProvider {
 public:
  Provider(std::vector<collision_detector::Item> items,
           std::vector<collision_detector::Gatherer> gatherers)
      : items_(items), gatherers_(gatherers) {
  }

  //Methods
  size_t ItemsCount() const override {
    return items_.size();
  }

  size_t GatherersCount() const override {
    return gatherers_.size();
  }

  collision_detector::Item GetItem(size_t pos) const override {
    return items_.at(pos);
  }

  collision_detector::Gatherer GetGatherer(size_t pos) const override {
    return gatherers_.at(pos);
  }

 private:
  std::vector<collision_detector::Item> items_;
  std::vector<collision_detector::Gatherer> gatherers_;
};

TEST_CASE("NO ITEMS") {
  std::vector<collision_detector::Item> items{};
  std::vector<collision_detector::Gatherer> gatherers{{{-1, 1}, {-2, 2}, 1.0}, {{2, -2}, {3, -3}, 2.0}, {{3, -3}, {-4, 4}, 3.0}};

  Provider provider{items, gatherers};
  std::vector<collision_detector::GatheringEvent> gathering_events = collision_detector::FindGatherEvents(provider);

  CHECK(gathering_events.empty());
}

TEST_CASE("NO GATHERERS") {
  std::vector<collision_detector::Item> items{{{-1, 1}, 1.0}, {{2, -2}, 2.0}, {{3, -3}, 3.0}};
  std::vector<collision_detector::Gatherer> gatherers{};

  Provider provider{items, gatherers};
  std::vector<collision_detector::GatheringEvent> gathering_events = collision_detector::FindGatherEvents(provider);

  CHECK(gathering_events.empty());
}

TEST_CASE("NO COLLISIONS") {
  std::vector<collision_detector::Item> items {{{-1, 1}, 1.0}, {{2, -2}, 2.0}, {{6, 2}, 2.0}, {{2, 6}, 1.0}};
  std::vector<collision_detector::Gatherer> gatherers{{{0, 6}, {-1, 6}, 1.0}};

  Provider provider{items, gatherers};
  std::vector<collision_detector::GatheringEvent> gathering_events = collision_detector::FindGatherEvents(provider);

  CHECK(gathering_events.empty());
}

TEST_CASE("COLLISIONS") {
  using Catch::Matchers::WithinRel;
  using Catch::Matchers::WithinAbs;

  std::vector<collision_detector::Item> items {{{-1, 1}, 1.0}, {{3, -2}, 2.0}, {{6, 2}, 2.0}, {{2, 6}, 1.0}};
  std::vector<collision_detector::Gatherer> gatherers {{{-4, 2}, {0, 2}, 2.0}, {{6, 6}, {6, 0}, 1.0}, {{2.5, 8}, {2.5, -4}, 1.0}};

  Provider provider{items, gatherers};
  std::vector<collision_detector::GatheringEvent> gathering_events = collision_detector::FindGatherEvents(provider);

  REQUIRE_FALSE(gathering_events.empty());
  CHECK(gathering_events.size() == 4);

  CHECK(gathering_events[0].gatherer_id == 2);
  CHECK(gathering_events[0].item_id == 3);
  CHECK_THAT(gathering_events[0].sq_distance, WithinRel(0.25, 1e-10));
  CHECK_THAT(gathering_events[0].time,  WithinRel(0.1666667, 1e-6));

  CHECK(gathering_events[1].gatherer_id == 1);
  CHECK(gathering_events[1].item_id == 2);
  CHECK_THAT(gathering_events[1].sq_distance, WithinAbs(0.0, 1e-10));
  CHECK_THAT(gathering_events[1].time, WithinRel(0.666667, 1e-6));

  CHECK(gathering_events[2].gatherer_id == 0);
  CHECK(gathering_events[2].item_id == 0);
  CHECK_THAT(gathering_events[2].sq_distance, WithinRel(1.0, 1e-10));
  CHECK_THAT(gathering_events[2].time, WithinRel(0.75, 1e-6));

  CHECK(gathering_events[3].gatherer_id == 2);
  CHECK(gathering_events[3].item_id == 1);
  CHECK_THAT(gathering_events[3].sq_distance, WithinRel(0.25, 1e-10));
  CHECK_THAT(gathering_events[3].time, WithinRel(0.8333333, 1e-6));
}
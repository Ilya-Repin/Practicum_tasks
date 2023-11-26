#include "collision_detector.h"
#include <cassert>
#include <iostream>

namespace collision_detector {

CollectionResult TryCollectPoint(geom::Point2D a, geom::Point2D b, geom::Point2D c) {
  const double u_x = c.x - a.x;
  const double u_y = c.y - a.y;
  const double v_x = b.x - a.x;
  const double v_y = b.y - a.y;
  const double u_dot_v = u_x * v_x + u_y * v_y;
  const double u_len2 = u_x * u_x + u_y * u_y;
  const double v_len2 = v_x * v_x + v_y * v_y;
  const double proj_ratio = u_dot_v / v_len2;
  const double sq_distance = u_len2 - (u_dot_v * u_dot_v) / v_len2;

  return CollectionResult{sq_distance, proj_ratio};
}

std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider &provider) {
  std::vector<GatheringEvent> events;

  for (std::size_t i = 0; i < provider.GatherersCount(); ++i) {
    Gatherer gatherer = provider.GetGatherer(i);

    if (gatherer.end_pos.x == gatherer.start_pos.x && gatherer.end_pos.y == gatherer.start_pos.y) {
      continue;
    }

    for (std::size_t j = 0; j < provider.ItemsCount(); ++j) {
      Item item = provider.GetItem(j);
      auto res = TryCollectPoint(gatherer.start_pos, gatherer.end_pos, item.position);

      if (res.IsCollected(gatherer.width + item.width)) {
        events.push_back({j, i, res.sq_distance, res.proj_ratio});
      }
    }
  }

  std::sort(events.begin(),
            events.end(),
            [](const GatheringEvent &event1, const GatheringEvent &event2) { return event1.time < event2.time; });

  return events;
}

}  // namespace collision_detector
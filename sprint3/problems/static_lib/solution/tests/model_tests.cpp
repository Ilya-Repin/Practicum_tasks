#include <cmath>
#include <catch2/catch_test_macros.hpp>

#include "../src/model/model.h"

using namespace std::literals;

SCENARIO("Game") {
  GIVEN("Game object") {
    model::Game game(model::LootGeneratorConfig{});

    WHEN("There are no maps") {
      CHECK(game.GetMaps().empty());
    };

    WHEN("There are maps") {
      boost::json::array arr1 = {{{"name", "key"}, {"file", "assets/key.obj"},
                                  {"type", "obj"},
                                  {"rotation", 90},
                                  {"color", "#338844"},
                                  {"scale", 3E-2}
                                 },
                                 {"{name", "wallet"},
                                 {"file", "assets/wallet.obj"},
                                 {"type", "obj"},
                                 {"rotation", 0},
                                 {"color", "#883344"},
                                 {"scale", 1E-2}
      };

      boost::json::array arr2 = {{{"name", "key"}, {"file", "assets/key.obj"},
                                  {"type", "obj"},
                                  {"rotation", 90},
                                  {"color", "#338844"},
                                  {"scale", 3E-2}
                                 },
                                 {"{name", "wallet"},
                                 {"file", "assets/wallet.obj"},
                                 {"type", "obj"},
                                 {"rotation", 0},
                                 {"color", "#883344"},
                                 {"scale", 1E-2}
      };

      model::Map map1(model::Map::Id{"1"}, "map_1", 15.0, ExtraData(arr1));
      game.AddMap(map1);
      model::Map map2(model::Map::Id{"2"}, "map_2", 20.0, ExtraData(arr2));
      game.AddMap(map2);

      REQUIRE_FALSE(game.GetMaps().empty());
      CHECK(game.FindMap(model::Map::Id{"1"}));
      CHECK(game.FindMap(model::Map::Id{"1"})->GetDogSpeed() == 15.0);
      CHECK(game.FindMap(model::Map::Id{"2"}));
      CHECK(game.FindMap(model::Map::Id{"2"})->GetDogSpeed() == 20.0);
    }

  }
}
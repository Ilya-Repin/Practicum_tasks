#include <cmath>
#include <catch2/catch_test_macros.hpp>

#include "../src/model/model.h"
#include "../src/model/game_session.h"

using namespace std::literals;

SCENARIO("Game") {
  GIVEN("Game object") {
    model::LootGeneratorConfig loot_config = {std::chrono::milliseconds(5), 0.5};
    model::Game game(loot_config);

    WHEN("There are no maps") {
      CHECK(game.GetMaps().empty());
    };

    WHEN("There are maps") {
      boost::json::array arr1 = {{{"name", "key"}, {"file", "assets/key.obj"},
                                  {"type", "obj"},
                                  {"rotation", 90},
                                  {"color", "#338844"},
                                  {"scale", 3E-2},
                                  {"value", 10}
                                 },
                                 {{"name", "wallet"},
                                  {"file", "assets/wallet.obj"},
                                  {"type", "obj"},
                                  {"rotation", 0},
                                  {"color", "#883344"},
                                  {"scale", 1E-2},
                                  {"value", 5}}
      };

      boost::json::array arr2 = {{{"name", "key"}, {"file", "assets/key.obj"},
                                  {"type", "obj"},
                                  {"rotation", 90},
                                  {"color", "#338844"},
                                  {"scale", 3E-2}, {"value", 10}
                                 },
                                 {{"name", "wallet"},
                                  {"file", "assets/wallet.obj"},
                                  {"type", "obj"},
                                  {"rotation", 0},
                                  {"color", "#883344"},
                                  {"scale", 1E-2}, {"value", 5}}
      };

      model::Map map1(model::Map::Id{"1"}, "map_1", 15.0, 3, ExtraData(arr1));
      REQUIRE_NOTHROW(game.AddMap(map1));
      model::Map map2(model::Map::Id{"2"}, "map_2", 20.0, 3, ExtraData(arr2));
      REQUIRE_NOTHROW(game.AddMap(map2));

      SECTION("Basic abilities of model") {
        CHECK_FALSE(game.GetMaps().empty());

        CHECK(game.FindMap(model::Map::Id{"1"}));
        CHECK(game.FindMap(model::Map::Id{"1"})->GetDogSpeed() == 15.0);
        CHECK_FALSE(game.FindMap(model::Map::Id{"1"})->GetExtraData().GetJsonArray().empty());

        REQUIRE(game.FindMap(model::Map::Id{"2"}));
        REQUIRE(game.FindMap(model::Map::Id{"2"})->GetDogSpeed() == 20.0);
        REQUIRE_FALSE(game.FindMap(model::Map::Id{"2"})->GetExtraData().GetJsonArray().empty());
      }

      auto road1 = model::Road(model::Road::HORIZONTAL, {0, 0}, 9);
      map1.AddRoad(road1);
      map1.AddLootValue(10);
      map1.AddLootValue(5);

      model::GameSession session(map1, model::GameSession::Id(0), false, game.GetLootGeneratorConfig());


      SECTION("Moving dog") {

        CHECK(*session.GetId() == 0);
        CHECK(*session.GetMapId() == "1");
        CHECK(session.CanAddPlayer());
        CHECK(session.GetDogs().empty());
        CHECK(session.GetLoots().empty());
        CHECK(session.GetPlayersData().empty());

        std::string dog_name = "Ben"s;
        auto dog = session.CreateDog(dog_name);
        auto velocity = model::Velocity();

        CHECK(dog->GetName() == dog_name);
        CHECK(*dog->GetId() == 1);
        CHECK(dog->GetPosition().x == 0);
        CHECK(dog->GetPosition().y == 0);
        CHECK(dog->GetDirection() == model::Direction::NORTH);
        CHECK(dog->GetVelocity() == velocity);

        dog->SetDirection('R');
        session.Tick(5);
        CHECK(dog->GetPosition().x == 0.075);
        CHECK(dog->GetPosition().y == 0);
        CHECK(dog->GetDirection() == model::Direction::EAST);
        CHECK(dog->GetVelocity().horizontal == 15.0);
        CHECK(dog->GetVelocity().vertical == 0.0);
      }

      SECTION("Catching loot") {
        std::string dog_name = "Rex"s;
        auto dog = session.CreateDog(dog_name);

        session.Tick(5);
        dog->SetDirection('R');
        session.Tick(5000);

        CHECK(dog->GetBag().size() == 1);
        CHECK(dog->GetScore() == 0);
        CHECK_FALSE(dog->IsBagFull());
      }
    }

  }
}
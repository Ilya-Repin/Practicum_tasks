#include "postgres.h"

#include <pqxx/zview.hxx>
#include <pqxx/pqxx>

namespace postgres {

using namespace std::literals;
using pqxx::operator "" _zv;

void RetiredDogRepositoryImpl::Save(const app::Retired &retired_dog) {
  auto connection = connection_pool_.GetConnection();
  pqxx::work work{*connection};

  work.exec_params(R"(INSERT INTO retired_players (id, name, score, play_time_ms) VALUES ($1, $2, $3, $4);)"_zv,
      retired_dog.id.ToString(), retired_dog.name, retired_dog.score, retired_dog.play_time_ms);

  work.commit();
}

std::vector<app::Retired> RetiredDogRepositoryImpl::GetRetiredDogs(int start, int max_items) {
  auto connection = connection_pool_.GetConnection();
  std::vector<app::Retired> retired_dogs;

  pqxx::read_transaction r{*connection};
  auto query_text =
      "SELECT id, name, score, play_time_ms FROM retired_players ORDER BY score DESC, play_time_ms, name OFFSET "
          + std::to_string(start) + " LIMIT " + std::to_string(max_items) + ";";

  auto res = r.query<std::string, std::string, uint64_t, uint64_t>(query_text);

  for (const auto [id, name, score, time] : res) {
    app::Retired retired_dog{app::RetiredId::FromString(id), name, score, time};
    retired_dogs.push_back(std::move(retired_dog));
  }

  return retired_dogs;
}

Database::Database(std::size_t capacity, std::string &db_url) : connection_pool_{capacity, [db_url] {
  return std::make_shared<pqxx::connection>(db_url);}} {

  auto conn = connection_pool_.GetConnection();
  pqxx::work work{*conn};

  work.exec(R"(
    CREATE TABLE IF NOT EXISTS retired_players (
    id UUID CONSTRAINT retired_dog_id_constraint PRIMARY KEY,
    name varchar(100) NOT NULL,
    score integer NOT NULL,
    play_time_ms integer NOT NULL);
    )"_zv);

  work.exec(R"(CREATE INDEX IF NOT EXISTS retired_players_index ON retired_players (score DESC, play_time_ms, name);)"_zv);
  work.commit();
}

}  // namespace postgres
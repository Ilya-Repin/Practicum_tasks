#pragma once

#include <pqxx/connection>
#include <pqxx/transaction>
#include "connection_pool.h"
#include "../app/repository.h"
#include "../app/Player.h"

namespace postgres {

class RetiredDogRepositoryImpl : public app::RetiredDogRepository {
 public:
  explicit RetiredDogRepositoryImpl(ConnectionPool &connection_pool)
      : connection_pool_{connection_pool} {
  }

  void Save(const app::Retired &retired_dog) override;
  std::vector<app::Retired> GetRetiredDogs(int start, int max_items) override;

 private:
  ConnectionPool& connection_pool_;
};

class Database {
 public:
  explicit Database(std::size_t capacity, std::string &db_url);

  RetiredDogRepositoryImpl &GetRetiredDogs() &{
    return retired_dogs_;
  }

 private:
  ConnectionPool connection_pool_;
  RetiredDogRepositoryImpl retired_dogs_{connection_pool_};
};

}  // namespace postgres
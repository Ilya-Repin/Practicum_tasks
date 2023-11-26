#pragma once

#include <boost/json/array.hpp>

class ExtraData {
 public:
  ExtraData(boost::json::array& array) : array_(array), loot_types_amount_(array.size()) {
  }

  int GetLootTypesAmount() const noexcept;
  const boost::json::array &GetJsonArray() const;

 private:
  size_t loot_types_amount_;
  boost::json::array array_;
};

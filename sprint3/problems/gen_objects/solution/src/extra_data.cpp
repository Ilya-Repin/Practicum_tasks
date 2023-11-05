#include "extra_data.h"
int ExtraData::GetLootTypesAmount() const noexcept {
  return loot_types_amount_;
}

const boost::json::array &ExtraData::GetJsonArray() const {
  return array_;
}

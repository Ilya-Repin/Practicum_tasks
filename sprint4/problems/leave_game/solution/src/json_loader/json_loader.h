#pragma once

#include <filesystem>
#include <boost/json.hpp>
#include <fstream>
#include <iostream>

#include "../model/model.h"
#include "../constants_storage.h"

namespace json_loader {

namespace json = boost::json;
using namespace model;
using namespace std::literals;

model::Game LoadGame(const std::filesystem::path &json_path);

}  // namespace json_loader
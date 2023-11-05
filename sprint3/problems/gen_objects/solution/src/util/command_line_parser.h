#pragma once

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

namespace util {

struct Args {
  unsigned int tick_period = 0;
  bool randomize_spawn_point = false;
  fs::path config_file;
  fs::path www_root;
};

std::optional<Args> ParseCommandLine(int argc, const char *const argv[]);

} // util
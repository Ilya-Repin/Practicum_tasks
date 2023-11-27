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
  fs::path state_file;
  unsigned int save_state_period = 0;
};

std::optional<Args> ParseCommandLine(int argc, const char *const argv[]);

} // util
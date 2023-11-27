#include "command_line_parser.h"

using namespace std::literals;

namespace util {
[[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char *const argv[]) {
  namespace po = boost::program_options;

  po::options_description desc{"Allowed options"s};

  Args args;
  desc.add_options()

      ("help,h", "produce help message")

      ("tick-period,t", po::value(&args.tick_period)->multitoken()->value_name("milliseconds"s), "set tick period")

      ("config-file,c", po::value(&args.config_file)->value_name("file"s), "set config file path")

      ("www-root,w", po::value(&args.www_root)->value_name("dir"s), "set static files root")

      ("randomize-spawn-points", po::value(&args.randomize_spawn_point), "spawn dogs at random positions")

      ("state-file", po::value(&args.state_file)->value_name("file"s), "set state file path")

      ("save-state-period",
       po::value(&args.save_state_period)->multitoken()->value_name("milliseconds"s),
       "set save state period");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.contains("help"s)) {
    std::cout << desc;
    return std::nullopt;
  }

  if (!vm.contains("config-file"s)) {
    throw std::runtime_error("Config file has not been specified");
  }

  if (!vm.contains("www-root"s)) {
    throw std::runtime_error("Static files path is not specified");
  }

  return args;
}

} // namespace util
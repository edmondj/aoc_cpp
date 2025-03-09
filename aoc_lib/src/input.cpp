#include "aoc_lib/input.hpp"

#include <CLI/CLI.hpp>

#include <fstream>

namespace aoc {

namespace {
const std::map<std::string, part> str_to_part{
    {"1", part::one}, {"one", part::one}, {"2", part::two}, {"two", part::two}};

std::string read_whole_file(const std::filesystem::path &path) {
  constexpr size_t read_size = 4096;
  auto in = std::ifstream(path);
  in.exceptions(std::ifstream::badbit);

  std::string out;
  char buff[read_size];
  do {
    in.read(buff, read_size);
    out.append(buff, in.gcount());
  } while (in.gcount() > 0);
  return out;
}
} // namespace

arguments parse_arguments(int ac, const char **av, const char *app_name) {
  std::optional<std::filesystem::path> input;
  std::optional<std::filesystem::path> output;

  arguments args;

  CLI::App app("An advent of code day", app_name ? app_name : av[0]);
  app.add_option("-i,--input", input, "Input file, defaults to input.txt");
  app.add_option("-e,--example", args.is_example,
                 "Flag that we're running an example");
  app.add_option("-p,--part", args.selected_part,
                 "Which part to run, defaults to both")
      ->transform(CLI::CheckedTransformer(str_to_part, CLI::ignore_case));
  app.add_option("-x,--expected", output,
                 "File containing the expected output, used for testing");
  try {
    app.parse(ac, av);
  } catch (const CLI::CallForHelp &) {
    std::cout << app.help();
    std::exit(1);
  }

  args.input = read_whole_file(input.value_or("input.txt"));
  args.expected_output = output.transform(read_whole_file);

  return args;
}

} // namespace aoc
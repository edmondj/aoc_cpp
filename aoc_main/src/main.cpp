#include "aoc_main/main.hpp"

#include <aoc_lib/input.hpp>
#include <iostream>

namespace aoc {

void display_exception() {
  try {
    throw;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
    std::cerr << "Unknown exception\n";
  }
}

int handle_result(const aoc::arguments &args, const std::string &out) {
  if (args.expected_output) {
    if (args.expected_output != out) {
      std::cout << "Expectation failed\n"
                   "- Expected:\n"
                << *args.expected_output << "- Got:\n"
                << out << '\n';
      return 1;
    }
  } else {
    std::cout << out << std::flush;
  }
  return 0;
}
} // namespace aoc
#pragma once

#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/input.hpp>

#include <sstream>

#define AOC_MAIN(trait)                                                        \
  int main(int ac, const char **av) try {                                      \
    const aoc::arguments &args = aoc::parse_arguments(ac, av, #trait);         \
    std::string output;                                                        \
    aoc::execute_day<trait>(args, std::back_inserter(output));                 \
    return aoc::handle_result(args, output);                                   \
  } catch (...) {                                                              \
    aoc::display_exception();                                                  \
    return -1;                                                                 \
  }

namespace aoc {
int handle_result(const aoc::arguments &args, const std::string &output);
void display_exception();
} // namespace aoc
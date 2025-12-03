#include "device.hpp"

#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <print>
#include <ranges>
#include <vector>

using namespace device;
using reg_t = registers_t<6>;

struct d19 {
  static program_t convert(std::string_view input) {
    return parse_program(input);
  }

  static value_t part1(const program_t &p) {
    auto regs = reg_t{};
    while (true) {
      value_t &ip = regs[*p.ip];
      evaluate(p.instructions[ip], regs);
      if (ip < 0 || static_cast<size_t>(ip) >= p.instructions.size() - 1) {
        break;
      }
      ++ip;
    }
    return regs[0];
  }

  static value_t part2(const program_t &) {
    // Reversed engineer my input and got the following code
    const value_t REG_5 = 10551417;

    value_t result = 0;
    for (value_t i = 1; i <= REG_5; ++i) {
      if (REG_5 % i == 0) {
        result += i;
      }
    }
    return result;
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d19)

#else

#include <gtest/gtest.h>

const auto test_data = aoc::arguments::make_example(R"(
#ip 0
seti 5 0 1
seti 6 0 2
addi 0 1 0
addr 1 2 3
setr 1 0 0
seti 8 0 4
seti 9 0 5
)");

TEST(d19, part1) { EXPECT_EQ(aoc::part1<d19>(test_data), 6); }

#endif
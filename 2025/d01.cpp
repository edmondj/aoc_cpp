#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/string.hpp>

#include <string>

struct d01 {

  static auto convert(const std::string &input) {
    return std::vector{
        std::from_range,
        aoc::lines(aoc::trimmed(input)) |
            std::views::transform([](std::string_view line) {
              int32_t sign = line[0] == 'L' ? -1 : 1;
              return sign * aoc::from_chars<int32_t>(line.substr(1)).value();
            })};
  }

  static auto run(const auto &input) {
    int32_t value = 50;
    size_t at_zero = 0;
    size_t passed_by_zero = 0;
    for (const auto &rot : input) {
      passed_by_zero += std::abs(rot) / 100;
      const auto turn = rot % 100;
      if (value != 0 && ((value + turn) >= 100 || (value + turn) <= 0)) {
        passed_by_zero += 1;
      }
      value = (value + turn) % 100;
      if (value < 0) {
        value += 100;
      }
      if (value == 0) {
        ++at_zero;
      }
    }
    return std::make_pair(at_zero, passed_by_zero);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d01)

#else

#include <gtest/gtest.h>

const auto TEST_DATA = aoc::arguments::make_example(R"(L68
L30
R48
L5
R60
L55
L1
L99
R14
L82)");

TEST(d01, part1) { EXPECT_EQ(aoc::part1<d01>(TEST_DATA), 3); }
TEST(d01, part2) { EXPECT_EQ(aoc::part2<d01>(TEST_DATA), 6); }

#endif
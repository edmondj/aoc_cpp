#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/string.hpp>

#include <algorithm>
#include <ranges>

struct d05 {
  static std::string convert(const std::string &input) {
    return std::string(aoc::trimmed(input));
  }

  static size_t part1(std::string input) {
    for (size_t i = 0; i + 1 < input.size();) {
      if (std::abs(input[i] - input[i + 1]) == 'a' - 'A') {
        input.erase(i, 2);
        if (i > 0) {
          --i;
        }
      } else {
        ++i;
      }
    }
    return input.size();
  }

  static size_t part2(const std::string &input) {
    size_t min = std::numeric_limits<size_t>::max();
    for (char to_remove : "ABCDEFGHIJKLMNOPQRSTUVWXYZ") {
      std::string modified = input;
      auto removed = std::erase_if(modified, [to_remove](char c) {
        return c == to_remove || (c - ('a' - 'A')) == to_remove;
      });
      if (removed == 0) {
        continue;
      }
      min = std::min(min, part1(std::move(modified)));
    }
    return min;
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>
AOC_MAIN(d05)

#else

#include <gtest/gtest.h>
TEST(d05, part1) {
  EXPECT_EQ(d05::part1("aA"), 0);
  EXPECT_EQ(d05::part1("abBA"), 0);
  EXPECT_EQ(d05::part1("abAB"), 4);
  EXPECT_EQ(d05::part1("aabAAB"), 6);
  EXPECT_EQ(d05::part1("dabAcCaCBAcCcaDA"), 10);
}

TEST(d05, part2) { EXPECT_EQ(d05::part2("dabAcCaCBAcCcaDA"), 4); }

#endif
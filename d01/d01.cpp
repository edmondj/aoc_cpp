#include <aoc_lib/string.hpp>

#include <algorithm>
#include <charconv>
#include <format>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

struct d01 {

  static std::vector<int64_t> convert(const std::string &input) {
    return std::vector(std::from_range,
                       std::views::split(aoc::trimmed(input), '\n') |
                           std::views::transform([](const auto &number_range) {
                             int64_t value;
                             const char *start = number_range.data();
                             const char *end =
                                 number_range.data() + number_range.size();
                             if (start != end && *start == '+') {
                               ++start;
                             }
                             auto result = std::from_chars(start, end, value);
                             if (result.ec != std::errc{}) {
                               throw std::runtime_error(std::format(
                                   "Failed to parse number {}: {}",
                                   std::string_view(number_range),
                                   std::make_error_code(result.ec).message()));
                             }
                             return value;
                           }));
  }

  static int64_t part1(const std::vector<int64_t> &input) {
    return std::ranges::fold_left(input, 0, std::plus<>{});
  }

  static int64_t part2(const std::vector<int64_t> &input) {
    auto current = int64_t{0};
    auto signals = std::unordered_set<int64_t>{};
    signals.insert(current);

    while (true) {
      for (int64_t delta : input) {
        current += delta;
        if (signals.contains(current)) {
          return current;
        }
        signals.insert(current);
      }
    }
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d01)

#else

#include <gtest/gtest.h>

TEST(d01, part1) {
  EXPECT_EQ(d01::part1({+1, -2, +3, +1}), 3);
  EXPECT_EQ(d01::part1({+1, +1, +1}), 3);
  EXPECT_EQ(d01::part1({+1, +1, -2}), 0);
  EXPECT_EQ(d01::part1({-1, -2, -3}), -6);
}

TEST(d01, part2) {
  EXPECT_EQ(d01::part2({+1, -2, +3, +1}), 2);
  EXPECT_EQ(d01::part2({+1, -1}), 0);
  EXPECT_EQ(d01::part2({+3, +3, +4, -2, -4}), 10);
  EXPECT_EQ(d01::part2({-6, +3, +8, +5, -6}), 5);
  EXPECT_EQ(d01::part2({+7, +7, -2, -7, -4}), 14);
}

#endif
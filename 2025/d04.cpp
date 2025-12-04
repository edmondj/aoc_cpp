#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/algorithm.hpp>
#include <aoc_lib/geometry/dyn_matrix.hpp>
#include <aoc_lib/string.hpp>

#include <string>
#include <unordered_set>

using map = aoc::dyn_matrix<char>;

struct d04 {
  static map convert(const std::string &input) {
    return aoc::dyn_matrix(std::from_range, aoc::lines(aoc::trimmed(input)));
  }

  static bool can_be_removed(const map &input, const map::point_t &point) {
    return input[point] == '@' &&
           std::ranges::count_if(
               aoc::adjacent(aoc::adjacent_type::euclidean,
                             aoc::point2d<std::ptrdiff_t>(point)),
               [&input](const auto &a) {
                 return a.x() >= 0 && a.y() >= 0 && input.contains(a) &&
                        input[a] == '@';
               }) < 4;
  }

  static auto run(map input) {
    size_t first_remove = 0;
    size_t total_remove = 0;
    auto to_remove = std::unordered_set<map::point_t>{};

    do {
      to_remove = std::unordered_set{
          std::from_range,
          aoc::views::point2d_iota(input.width(), input.height()) |
              std::views::filter([&input](const auto &point) {
                return can_be_removed(input, point);
              })};
      for (const auto &p : to_remove) {
        input[p] = '.';
      }
      if (first_remove == 0) {
        first_remove = to_remove.size();
      }
      total_remove += to_remove.size();
    } while (!to_remove.empty());

    return std::make_pair(first_remove, total_remove);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d04)

#else

#include <gtest/gtest.h>

const auto TEST_DATA = aoc::arguments::make_example(R"(..@@.@@@@.
@@@.@.@.@@
@@@@@.@.@@
@.@@@@..@.
@@.@@@@.@@
.@@@@@@@.@
.@.@.@.@@@
@.@@@.@@@@
.@@@@@@@@.
@.@.@@@.@.
)");

TEST(d04, part1) { EXPECT_EQ(aoc::part1<d04>(TEST_DATA), 13); }
TEST(d04, part2) { EXPECT_EQ(aoc::part2<d04>(TEST_DATA), 43); }

#endif
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/dyn_matrix.hpp>
#include <aoc_lib/geometry/vector.hpp>
#include <aoc_lib/string.hpp>

#include <string>
#include <unordered_map>
#include <unordered_set>

#include <aoc_lib/geometry_format.hpp>
#include <print>

using input_t = aoc::dyn_matrix<char>;
using point_t = input_t::point_t;

struct d07 {

  static auto convert(const std::string &input) -> input_t {
    return {std::from_range, aoc::lines(aoc::trimmed(input))};
  }

  static auto count_timelines_from(const input_t &input, point_t start,
                                   std::unordered_set<point_t> &split_points,
                                   std::unordered_map<point_t, size_t> &cache) {
    if (auto found = cache.find(start); found != cache.end()) {
      return found->second;
    }
    auto cur = start;
    do {
      cur.y() += 1;
    } while (cur.y() < input.height() && input[cur] != '^');
    if (cur.y() >= input.height()) {
      return 1uz;
    }
    split_points.insert(cur);
    auto res = 0uz;
    if (cur.x() > 0) {
      res += count_timelines_from(input, {cur.x() - 1, cur.y()}, split_points,
                                  cache);
    }
    if (cur.x() < input.width() - 1) {
      res += count_timelines_from(input, {cur.x() + 1, cur.y()}, split_points,
                                  cache);
    }
    cache.insert_or_assign(start, res);
    return res;
  }

  static auto run(const input_t &input) {
    auto start =
        point_t{*std::ranges::find_if(
                    std::views::iota(0uz, input.width()),
                    [&input](size_t x) { return input[{x, 0}] == 'S'; }),
                0};
    auto split_points = std::unordered_set<point_t>{};
    auto cache = std::unordered_map<point_t, size_t>{};
    auto part2 = count_timelines_from(input, start, split_points, cache);
    return std::make_pair(split_points.size(), part2);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d07)

#else

#include <gtest/gtest.h>

const auto TEST_DATA = aoc::arguments::make_example(R"(.......S.......
...............
.......^.......
...............
......^.^......
...............
.....^.^.^.....
...............
....^.^...^....
...............
...^.^...^.^...
...............
..^...^.....^..
...............
.^.^.^.^.^...^.
...............
)");

TEST(d07, part1) { EXPECT_EQ(aoc::part1<d07>(TEST_DATA), 21); }
TEST(d07, part2) { EXPECT_EQ(aoc::part2<d07>(TEST_DATA), 40); }

#endif
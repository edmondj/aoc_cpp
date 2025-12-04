#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/algorithm.hpp>
#include <aoc_lib/geometry/dyn_matrix.hpp>
#include <aoc_lib/string.hpp>

#include <string>
#include <unordered_map>
#include <unordered_set>

using map = aoc::dyn_matrix<char>;

struct d04 {
  static map convert(const std::string &input) {
    return aoc::dyn_matrix(std::from_range, aoc::lines(aoc::trimmed(input)));
  }

  static size_t count_neighbours(const map &input, const map::point_t &point) {
    return std::ranges::count_if(
        aoc::adjacent(aoc::adjacent_type::euclidean,
                      aoc::point2d<std::ptrdiff_t>(point)),
        [&input](const auto &a) {
          return a.x() >= 0 && a.y() >= 0 && input.contains(a) &&
                 input[a] == '@';
        });
  }

  static auto run(const map &input) {
    auto to_remove = std::unordered_set<map::point_t>{};
    auto cannot_remove = std::unordered_map<map::point_t, size_t>{};
    auto removed = std::unordered_set<map::point_t>{};

    for (const auto &[p, v] : input.enumerate()) {
      if (v == '@') {
        const auto neighbors = count_neighbours(input, p);
        if (neighbors < 4) {
          to_remove.insert(p);
        } else {
          cannot_remove.insert_or_assign(p, neighbors);
        }
      }
    }

    size_t first_removal = to_remove.size();

    while (!to_remove.empty()) {
      for (const map::point_t &cur : std::exchange(to_remove, {})) {
        if (removed.insert(cur).second) {
          for (auto a : aoc::adjacent(aoc::adjacent_type::euclidean,
                                      aoc::point2d<ptrdiff_t>(cur))) {
            auto found = cannot_remove.find(a);
            if (found != cannot_remove.end()) {
              if (--found->second < 4) {
                to_remove.insert(found->first);
                cannot_remove.erase(found);
              }
            }
          }
        }
      }
    }

    return std::make_pair(first_removal, removed.size());
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
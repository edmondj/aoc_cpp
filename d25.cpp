#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/point.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using value_t = int64_t;
using point_t = aoc::point<value_t, 4>;

using data_t = std::vector<point_t>;

struct d25 {
  static data_t convert(std::string_view input) {
    static const auto re = std::regex(R"(^(-?\d+),(-?\d+),(-?\d+),(-?\d+)$)");
    return std::vector(
        std::from_range,
        aoc::lines(aoc::trimmed(input)) |
            std::views::transform([](std::string_view line) {
              auto match = *aoc::regex_match(aoc::trimmed(line), re);
              return point_t({*aoc::from_chars<value_t>(match.str(1)),
                              *aoc::from_chars<value_t>(match.str(2)),
                              *aoc::from_chars<value_t>(match.str(3)),
                              *aoc::from_chars<value_t>(match.str(4))});
            }));
  }

  static size_t part1(const data_t &d) {
    size_t next_constellation = 0;
    auto point_constellation =
        std::vector<std::optional<size_t>>(d.size(), std::nullopt);
    auto all_constellations =
        std::unordered_map<size_t, std::unordered_set<ptrdiff_t>>();

    auto create_constellation =
        [&](std::unordered_set<ptrdiff_t> constellation) {
          for (size_t i : constellation) {
            point_constellation[i] = next_constellation;
          }
          all_constellations.emplace(next_constellation++,
                                     std::move(constellation));
        };

    auto merge_constellations = [&](size_t from, size_t to) {
      if (from != to) {
        for (ptrdiff_t i : all_constellations[from]) {
          point_constellation[i] = to;
        }
        all_constellations[to].insert_range(all_constellations[from]);
        all_constellations.erase(from);
      }
    };

    auto add_to_constellation = [&](ptrdiff_t i, size_t to) {
      point_constellation[i] = to;
      all_constellations[to].insert(i);
    };

    auto enumarated = d | std::views::enumerate;
    auto it = std::ranges::begin(enumarated);
    auto end = std::ranges::end(enumarated);
    while (it != end) {
      auto [idx, point] = *it;
      auto &my_constellation = point_constellation[idx];
      for (auto [nidx, npoint] : std::ranges::subrange(++it, end)) {
        if (aoc::manhattan_distance(point, npoint) <= 3) {
          auto &other_constellation = point_constellation[nidx];
          if (!my_constellation && !other_constellation) {
            // Whole new constellation
            create_constellation({idx, nidx});
          } else if (!my_constellation && other_constellation) {
            // Get absorbed in other
            add_to_constellation(idx, *other_constellation);
          } else if (my_constellation && !other_constellation) {
            // Absorb other in my constellation
            add_to_constellation(nidx, *my_constellation);
          } else {
            // Merge both constellations
            merge_constellations(*my_constellation, *other_constellation);
          }
        }
      }
      if (!my_constellation) {
        // Single point constellation
        create_constellation({idx});
      }
    }

    return all_constellations.size();
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d25)

#else

#include <gtest/gtest.h>

const auto test_data =
    std::vector<std::pair<const char *, size_t>>({std::make_pair(
                                                      R"(
 0,0,0,0
 3,0,0,0
 0,3,0,0
 0,0,3,0
 0,0,0,3
 0,0,0,6
 9,0,0,0
12,0,0,0
)",
                                                      2),
                                                  std::make_pair(
                                                      R"(
 0,0,0,0
 3,0,0,0
 0,3,0,0
 0,0,3,0
 0,0,0,3
 0,0,0,6
 9,0,0,0
12,0,0,0
 6,0,0,0
)",
                                                      1),
                                                  std::make_pair(
                                                      R"(
-1,2,2,0
0,0,2,-2
0,0,0,-2
-1,2,0,0
-2,-2,-2,2
3,0,2,-1
-1,3,2,2
-1,0,-1,0
0,2,1,-2
3,0,0,0

)",
                                                      4),
                                                  std::make_pair(
                                                      R"(
1,-1,0,1
2,0,-1,0
3,2,-1,0
0,0,3,1
0,0,-1,-1
2,3,-2,0
-2,2,0,0
2,-2,0,-1
1,-1,0,-1
3,2,0,2
)",
                                                      3),
                                                  std::make_pair(
                                                      R"(
1,-1,-1,-2
-2,-2,0,1
0,2,1,3
-2,3,-2,1
0,2,3,-2
-1,-1,1,-2
0,-2,-1,0
-2,2,3,-1
1,2,2,0
-1,-2,0,-2
)",
                                                      8)});

TEST(d25, part1) {
  for (const auto &[input, expected] : test_data) {
    EXPECT_EQ(aoc::part1<d25>(aoc::arguments::make_example(input)), expected)
        << input;
  }
}

#endif
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/dyn_matrix.hpp>
#include <aoc_lib/string.hpp>

#include <cassert>

using area_t = aoc::dyn_matrix<char>;
using point = aoc::point2d<size_t>;

struct stats_t {
  size_t tree = 0;
  size_t lumber = 0;

  void inspect(char c) {
    if (c == '|') {
      ++tree;
    } else if (c == '#') {
      ++lumber;
    }
  }
};

size_t score_of(const area_t &area) {
  stats_t stats;
  for (char c : area.data_view()) {
    stats.inspect(c);
  }
  return stats.tree * stats.lumber;
}

void iterate_area_to(const area_t &from, area_t &to) {
  assert(from.width() == to.width());
  assert(from.height() == to.height());

  for (auto [p, c] : from.enumerate()) {
    to[p] = [&from](point p, char c) {
      stats_t stats;
      if (p.x() > 0) {
        if (p.y() > 0) {
          stats.inspect(from[{p.x() - 1, p.y() - 1}]);
        }
        stats.inspect(from[{p.x() - 1, p.y()}]);
        if (p.y() + 1 < from.height()) {
          stats.inspect(from[{p.x() - 1, p.y() + 1}]);
        }
      }
      if (p.y() > 0) {
        stats.inspect(from[{p.x(), p.y() - 1}]);
      }
      if (p.y() + 1 < from.height()) {
        stats.inspect(from[{p.x(), p.y() + 1}]);
      }
      if (p.x() + 1 < from.width()) {
        if (p.y() > 0) {
          stats.inspect(from[{p.x() + 1, p.y() - 1}]);
        }
        stats.inspect(from[{p.x() + 1, p.y()}]);
        if (p.y() + 1 < from.height()) {
          stats.inspect(from[{p.x() + 1, p.y() + 1}]);
        }
      }

      switch (c) {
      case '.':
        return stats.tree >= 3 ? '|' : '.';
      case '|':
        return stats.lumber >= 3 ? '#' : '|';
      case '#':
        return stats.tree >= 1 && stats.lumber >= 1 ? '#' : '.';
      }
      std::unreachable();
    }(p, c);
  }
}

struct d18 {
  static area_t convert(std::string_view input) {
    size_t width = 0;
    size_t height = 0;
    std::vector<char> data;
    data.reserve(input.size());
    for (std::string_view line : aoc::lines(aoc::trimmed(input))) {
      if (width == 0) {
        width = line.size();
      }
      assert(width == line.size());
      ++height;
      data.append_range(line);
    }
    return area_t{width, height, std::move(data)};
  }

  static std::pair<size_t, size_t> run(area_t area) {
    const auto PART1 = 10uz;
    const auto PART2 = 1000000000uz;

    std::vector<area_t> history;
    area_t next = area;
    size_t cycle_start;
    size_t cycle_end;

    while (true) {
      auto found = std::ranges::find(history, area);
      if (found != history.end()) {
        cycle_start = found - history.begin();
        cycle_end = history.size();
        break;
      }
      history.push_back(area);
      iterate_area_to(area, next);
      std::swap(area, next);
    }

    return std::make_pair(
        score_of(history[PART1]),
        score_of(history[(PART2 - cycle_start) % (cycle_end - cycle_start) +
                         cycle_start]));
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d18)

#else

#include <gtest/gtest.h>

const auto test_data = aoc::arguments::make_example(R"(
.#.#...|#.
.....#|##|
.|..|...#.
..|#.....#
#.#|||#|#|
...#.||...
.|....|...
||...#|.#|
|.||||..|.
...#.|..|.
)");

TEST(d18, part1) { EXPECT_EQ(aoc::part1<d18>(test_data), 1147); }

#endif
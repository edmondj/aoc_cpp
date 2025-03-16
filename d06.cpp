#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/string.hpp>

#include <algorithm>
#include <format>
#include <ranges>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <vector>

struct point {
  int64_t x;
  int64_t y;
};

int64_t manhattan_distance(const point &l, const point &r) {
  return std::abs(l.x - r.x) + std::abs(l.y - r.y);
}

class closest_point_accumulator {
public:
  void accumulate(int64_t id, int64_t dist) {
    if (dist < min_dist) {
      min_dist = dist;
      closest = id;
    } else if (dist == min_dist) {
      closest = std::nullopt;
    }
  }

  std::optional<int64_t> get_closest() const { return closest; }

private:
  std::optional<int64_t> closest;
  int64_t min_dist = std::numeric_limits<int64_t>::max();
};

std::optional<int64_t>
closest_point(const point &from, const std::ranges::input_range auto &range) {
  closest_point_accumulator acc;
  for (const auto &[i, d] : range | std::views::enumerate) {
    acc.accumulate(i, manhattan_distance(from, d));
  }
  return acc.get_closest();
}

struct d06 {
  struct data {
    std::vector<point> points;
    std::int64_t part2_dist;
  };

  static data convert(const aoc::arguments &args) {
    return data{
        .points =
            std::vector{
                std::from_range,
                aoc::lines(aoc::trimmed(args.input)) |
                    std::views::transform([](std::string_view line) {
                      auto comma = line.find(", ");
                      if (comma == std::string_view::npos) {
                        throw std::runtime_error(
                            std::format("Failed to parse line {}", line));
                      }
                      return point{
                          *aoc::from_chars<int64_t>(line.substr(0, comma)),
                          *aoc::from_chars<int64_t>(line.substr(comma + 2))};
                    })},
        .part2_dist = args.is_example ? 32 : 10000};
  }

  static std::pair<int64_t, int64_t> run(const data &d) {
    auto top_left = point{std::numeric_limits<int64_t>::max(),
                          std::numeric_limits<int64_t>::max()};
    auto bottom_right = point{0, 0};
    for (const auto &[x, y] : d.points) {
      top_left.x = std::min(top_left.x, x);
      top_left.y = std::min(top_left.y, y);
      bottom_right.x = std::max(bottom_right.x, x);
      bottom_right.y = std::max(bottom_right.y, y);
    }

    // Remove infinit areas
    // Walk the perimeter, any one closest has infinite area
    std::set<int64_t> infinites;
    // Horizontal edges
    for (auto [top, bottom] :
         std::views::iota(top_left.x, bottom_right.x + 1) |
             std::views::transform([&](int64_t x) {
               return std::make_pair(
                   closest_point({x, top_left.y}, d.points),
                   closest_point({x, bottom_right.y}, d.points));
             })) {
      if (top)
        infinites.insert(*top);
      if (bottom)
        infinites.insert(*bottom);
    }
    // Vertical edges
    for (auto [left, right] :
         std::views::iota(top_left.y, bottom_right.y + 1) |
             std::views::transform([&](int64_t y) {
               return std::make_pair(
                   closest_point({top_left.x, y}, d.points),
                   closest_point({bottom_right.x, y}, d.points));
             })) {
      if (left)
        infinites.insert(*left);
      if (right)
        infinites.insert(*right);
    }

    // Compute areas
    std::unordered_map<std::size_t, std::size_t> areas;
    int64_t center_area = 0;

    for (int64_t x = top_left.x; x <= bottom_right.x; ++x) {
      for (int64_t y = top_left.y; y <= bottom_right.y; ++y) {
        closest_point_accumulator acc;
        int64_t dist_sum = 0;
        for (const auto &[i, p] : d.points | std::views::enumerate) {
          auto dist = manhattan_distance(point{x, y}, p);
          acc.accumulate(i, dist);
          dist_sum += dist;
        }
        if (auto closest = acc.get_closest()) {
          areas[*closest] += 1;
        }
        if (dist_sum < d.part2_dist) {
          center_area += 1;
        }
      }
    }

    return std::make_pair(
        std::ranges::max(areas |
                         std::views::filter([&infinites](const auto &entry) {
                           return !infinites.contains(entry.first);
                         }) |
                         std::views::values),
        center_area);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d06)

#else

#include <gtest/gtest.h>

namespace {
const aoc::arguments testData{.input = R"(
1, 1
1, 6
8, 3
3, 4
5, 5
8, 9
)",
                              .is_example = true};
} // namespace

TEST(d06, part1) { EXPECT_EQ(aoc::part1<d06>(testData), 17); }
TEST(d06, part2) { EXPECT_EQ(aoc::part2<d06>(testData), 16); }

#endif
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/dyn_matrix.hpp>
#include <aoc_lib/geometry/point.hpp>
#include <aoc_lib/string.hpp>

#include <set>
#include <string>
#include <unordered_map>

using point_t = aoc::point2d<int64_t>;
using input_t = std::vector<point_t>;

class segment_t {
public:
  segment_t(point_t from, point_t to)
      : m_from(std::min(from, to)), m_to(std::max(from, to)) {
    assert(from.x() == to.x() || from.y() == to.y());
  }

  const point_t &from() const { return m_from; }
  const point_t &to() const { return m_to; }

private:
  point_t m_from, m_to;
};

bool polygon_contains(std::span<const segment_t> segments, point_t p,
                      std::unordered_map<point_t, bool> &cache) {
  if (auto found = cache.find(p); found != cache.end()) {
    return found->second;
  }
  // Crossing number implementation, raycasting to the right (x positive)
  size_t count = 0;
  for (const auto &s : segments) {
    if (s.from().y() == s.to().y()) {
      if (s.from().y() == p.y()) {
        auto [low_x, high_x] = std::minmax(s.from().x(), s.to().x());
        if (low_x <= p.x() && p.x() <= high_x) {
          return true;
        }
      }
    } else if (s.from().x() >= p.x()) {
      auto [low_y, high_y] = std::minmax(s.from().y(), s.to().y());
      if (low_y <= p.y() && p.y() <= high_y) {
        if (s.from().x() == p.x()) {
          return true;
        }
        ++count;
      }
    }
  }
  bool res = count % 2 == 1;
  cache.insert_or_assign(p, res);
  return res;
}

bool polygon_contains(std::span<const segment_t> segments,
                      const std::set<point_t::value_type> &x_range,
                      const std::set<point_t::value_type> &y_range,
                      const segment_t &test,
                      std::unordered_map<point_t, bool> &cache) {
  if (test.from().x() == test.to().x()) {
    auto [low_y, high_y] = std::minmax(test.from().y(), test.to().y());
    return std::ranges::all_of(
        std::ranges::subrange(y_range.lower_bound(low_y),
                              y_range.upper_bound(high_y - 1)),
        [&cache, segments, x = test.from().x()](point_t::value_type y) {
          return polygon_contains(segments, {x, y + 1}, cache);
        });
  }
  auto [low_x, high_x] = std::minmax(test.from().x(), test.to().x());
  return std::ranges::all_of(
      std::ranges::subrange(x_range.lower_bound(low_x),
                            x_range.upper_bound(high_x - 1)),
      [&cache, segments, y = test.from().y()](point_t::value_type x) {
        return polygon_contains(segments, {x + 1, y}, cache);
      });
}

struct d09 {
  static auto convert(const std::string &input) -> input_t {
    return std::vector{
        std::from_range,
        aoc::lines(aoc::trimmed(input)) |
            std::views::transform([](std::string_view line) {
              return point_t{
                  std::from_range,
                  aoc::split(line, ',') |
                      std::views::transform([](std::string_view number) {
                        return aoc::from_chars<point_t::value_type>(number)
                            .value();
                      })};
            })};
  }

  static auto part1(const auto &input) {
    return std::ranges::max(
        std::views::iota(0uz, input.size() - 1) |
        std::views::transform([&input](size_t i) {
          return std::ranges::subrange(input.begin() + i + 1, input.end()) |
                 std::views::transform([cur = input[i]](const point_t &next) {
                   return (abs(cur.x() - next.x()) + 1) *
                          (abs(cur.y() - next.y()) + 1);
                 });
        }) |
        std::views::join);
  }

  static auto part2(const auto &input) {
    auto segments = std::vector{
        std::from_range, std::views::iota(0uz, input.size()) |
                             std::views::transform([&input](size_t i) {
                               return segment_t{input[i],
                                                input[(i + 1) % input.size()]};
                             })};

    std::set<point_t::value_type> x_range, y_range;
    for (const auto &p : input) {
      x_range.insert(p.x());
      y_range.insert(p.y());
    }
    auto cache = std::unordered_map<point_t, bool>{};

    return std::ranges::max(
        std::views::iota(0uz, input.size() - 1) |
        std::views::transform([&cache, &x_range, &y_range, &input,
                               &segments](size_t i) {
          return std::ranges::subrange(input.begin() + i + 1, input.end()) |
                 std::views::transform([&cache, &x_range, &y_range,
                                        cur = input[i],
                                        &segments](const point_t &next) {
                   auto extra_corners = std::array{point_t{cur.x(), next.y()},
                                                   point_t{next.x(), cur.y()}};
                   if (polygon_contains(segments, x_range, y_range,
                                        {cur, extra_corners[0]}, cache) &&
                       polygon_contains(segments, x_range, y_range,
                                        {extra_corners[0], next}, cache) &&
                       polygon_contains(segments, x_range, y_range,
                                        {next, extra_corners[1]}, cache) &&
                       polygon_contains(segments, x_range, y_range,
                                        {extra_corners[1], cur}, cache)) {
                     return (abs(cur.x() - next.x()) + 1) *
                            (abs(cur.y() - next.y()) + 1);
                   } else {
                     return 0ll;
                   }
                 });
        }) |
        std::views::join);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d09)

#else

#include <gtest/gtest.h>

// const auto TEST_DATA = aoc::arguments::make_example(R"(7,1
// 11,1
// 11,7
// 9,7
// 9,5
// 2,5
// 2,3
// 7,3)");
// TEST(d09, part1) { EXPECT_EQ(aoc::part1<d09>(TEST_DATA), 50); }
// TEST(d09, part2) { EXPECT_EQ(aoc::part2<d09>(TEST_DATA), 24); }

const auto TEST_DATA = aoc::arguments::make_example(R"(1,1
10,1
10,10
1,10
1,7
8,7
8,5
1,5)");
TEST(d09, part1) { EXPECT_EQ(aoc::part1<d09>(TEST_DATA), 100); }
TEST(d09, part2) { EXPECT_EQ(aoc::part2<d09>(TEST_DATA), 50); }

#endif
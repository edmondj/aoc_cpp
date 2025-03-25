#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <print>
#include <stdexcept>
#include <unordered_set>
#include <vector>

struct star {
  aoc::point2d<int64_t> position;
  aoc::vector2d<int64_t> velocity;
};

using data = std::vector<star>;

struct d10 {
  static data convert(std::string_view input) {
    return data{
        std::from_range,
        aoc::lines(aoc::trimmed(input)) | std::views::transform([](std::string_view
                                                                       line) {
          static const auto re = std::regex(
              R"(^position=<\s*(-?\d+), \s*(-?\d+)> velocity=<\s*(-?\d+), \s*(-?\d+)>$)");
          auto match = *aoc::regex_match(line, re);
          return star{.position = {*aoc::from_chars<int64_t>(match.str(1)),
                                   *aoc::from_chars<int64_t>(match.str(2))},
                      .velocity = {*aoc::from_chars<int64_t>(match.str(3)),
                                   *aoc::from_chars<int64_t>(match.str(4))}};
        })};
  }

  static std::pair<std::string, int64_t> run(const data &d) {
    const int64_t max_iteraton = 100000;

    auto compute_points_rms = [&d](int64_t t) {
      auto points = std::vector{std::from_range,
                                d | std::views::transform([t](const star &s) {
                                  return s.position + t * s.velocity;
                                })};
      auto centroid = *aoc::centroid(points);
      auto rms = *aoc::rms(points |
                           std::views::transform(
                               [&centroid](const aoc::point2d<int64_t> &point) {
                                 return aoc::amplitude(point - centroid);
                               }));
      return std::make_tuple(std::move(points), rms);
    };

    auto [points, rms] = compute_points_rms(0);

    for (int64_t t = 1; t < max_iteraton; ++t) {
      auto [next_points, next_rms] = compute_points_rms(t);
      if (next_rms >= rms) {
        aoc::vector2d<int64_t> slide_top_left{
            std::numeric_limits<int64_t>::max(),
            std::numeric_limits<int64_t>::max()};
        aoc::point2d<int64_t> bottom_right{std::numeric_limits<int64_t>::min(),
                                           std::numeric_limits<int64_t>::min()};
        for (const aoc::point2d<int64_t> &p : points) {
          slide_top_left.dx() = std::min(slide_top_left.dx(), p.x());
          slide_top_left.dy() = std::min(slide_top_left.dy(), p.y());
          bottom_right.x() = std::max(bottom_right.x(), p.x());
          bottom_right.y() = std::max(bottom_right.y(), p.y());
        }
        bottom_right -= slide_top_left;
        std::unordered_set<aoc::point2d<int64_t>> anchored_points;
        anchored_points.reserve(points.size());
        anchored_points.insert_range(
            points | std::views::transform(
                         [&slide_top_left](const aoc::point2d<int64_t> &p) {
                           return p - slide_top_left;
                         }));
        std::string result;
        result.reserve((bottom_right.x() + 2) * (bottom_right.y() + 1));
        for (int64_t y = 0; y <= bottom_right.y(); ++y) {
          for (int64_t x = 0; x <= bottom_right.x(); ++x) {
            result.push_back(anchored_points.contains({x, y}) ? '#' : '.');
          }
          result.push_back('\n');
        }
        return std::make_pair(result, t - 1);
      }
      points = std::move(next_points);
      rms = next_rms;
    }

    throw std::runtime_error(
        "Reached max iteration without finding a solution");
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d10)

#else

#include <gtest/gtest.h>

namespace {
const auto test_data = aoc::arguments::make_example(R"(
position=< 9,  1> velocity=< 0,  2>
position=< 7,  0> velocity=<-1,  0>
position=< 3, -2> velocity=<-1,  1>
position=< 6, 10> velocity=<-2, -1>
position=< 2, -4> velocity=< 2,  2>
position=<-6, 10> velocity=< 2, -2>
position=< 1,  8> velocity=< 1, -1>
position=< 1,  7> velocity=< 1,  0>
position=<-3, 11> velocity=< 1, -2>
position=< 7,  6> velocity=<-1, -1>
position=<-2,  3> velocity=< 1,  0>
position=<-4,  3> velocity=< 2,  0>
position=<10, -3> velocity=<-1,  1>
position=< 5, 11> velocity=< 1, -2>
position=< 4,  7> velocity=< 0, -1>
position=< 8, -2> velocity=< 0,  1>
position=<15,  0> velocity=<-2,  0>
position=< 1,  6> velocity=< 1,  0>
position=< 8,  9> velocity=< 0, -1>
position=< 3,  3> velocity=<-1,  1>
position=< 0,  5> velocity=< 0, -1>
position=<-2,  2> velocity=< 2,  0>
position=< 5, -2> velocity=< 1,  2>
position=< 1,  4> velocity=< 2,  1>
position=<-2,  7> velocity=< 2, -2>
position=< 3,  6> velocity=<-1, -1>
position=< 5,  0> velocity=< 1,  0>
position=<-6,  0> velocity=< 2,  0>
position=< 5,  9> velocity=< 1, -2>
position=<14,  7> velocity=<-2,  0>
position=<-3,  6> velocity=< 2, -1>
)");

const auto part1_result = R"(
#...#..###
#...#...#.
#...#...#.
#####...#.
#...#...#.
#...#...#.
#...#...#.
#...#..###
)";

} // namespace

TEST(d10, part1) {
  EXPECT_EQ(aoc::trimmed(aoc::part1<d10>(test_data)),
            aoc::trimmed(part1_result));
}

TEST(d10, part2) { EXPECT_EQ(aoc::part2<d10>(test_data), 3); }

#endif
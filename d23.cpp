#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/point.hpp>
#include <aoc_lib/geometry/vector.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <generator>
#include <ranges>
#include <set>

using value_t = int64_t;
using point_t = aoc::point3d<value_t>;
using vector_t = aoc::vector3d<value_t>;

constexpr auto ORIGIN = point_t{0, 0, 0};

struct sphere_t {
  point_t pos;
  value_t r;
};

// Cube is aligned on the axis
struct oriented_cube_t {
  point_t center;
  value_t half_size;
};

bool intersects(const sphere_t &sphere, const oriented_cube_t &cube) {
  auto c1 =
      cube.center + vector_t{-cube.half_size, -cube.half_size, -cube.half_size};
  auto c2 =
      cube.center + vector_t{cube.half_size, cube.half_size, cube.half_size};
  auto dist = sphere.r;
  if (sphere.pos.x() < c1.x())
    dist -= c1.x() - sphere.pos.x();
  else if (sphere.pos.x() > c2.x())
    dist -= sphere.pos.x() - c2.x();
  if (sphere.pos.y() < c1.y())
    dist -= c1.y() - sphere.pos.y();
  else if (sphere.pos.y() > c2.y())
    dist -= sphere.pos.y() - c2.y();
  if (sphere.pos.z() < c1.z())
    dist -= c1.z() - sphere.pos.z();
  else if (sphere.pos.z() > c2.z())
    dist -= sphere.pos.z() - c2.z();
  return dist >= 0;
}

struct search_space_t {
  size_t missing_bots;
  value_t distance; // cached distance to origin
  oriented_cube_t cube;

  auto cmp_tuple() const {
    return std::make_tuple(missing_bots, distance, cube.center.x(),
                           cube.center.y(), cube.center.z());
  }

  friend bool operator<(const search_space_t &l, const search_space_t &r) {
    return l.cmp_tuple() < r.cmp_tuple();
  }
};

std::generator<oriented_cube_t> split_search(const oriented_cube_t &cube) {
  const auto &[center, old_radius] = cube;
  if (old_radius == 1) {
    // if radius is one, inspect every single points
    co_yield {center + vector_t{0, 0, -1}, 0};
    co_yield {center + vector_t{0, 0, 0}, 0};
    co_yield {center + vector_t{0, 0, 1}, 0};
    co_yield {center + vector_t{0, -1, -1}, 0};
    co_yield {center + vector_t{0, -1, 0}, 0};
    co_yield {center + vector_t{0, -1, 1}, 0};
    co_yield {center + vector_t{0, 1, -1}, 0};
    co_yield {center + vector_t{0, 1, 0}, 0};
    co_yield {center + vector_t{0, 1, 1}, 0};
    co_yield {center + vector_t{-1, 0, -1}, 0};
    co_yield {center + vector_t{-1, 0, 0}, 0};
    co_yield {center + vector_t{-1, 0, 1}, 0};
    co_yield {center + vector_t{-1, -1, -1}, 0};
    co_yield {center + vector_t{-1, -1, 0}, 0};
    co_yield {center + vector_t{-1, -1, 1}, 0};
    co_yield {center + vector_t{-1, 1, -1}, 0};
    co_yield {center + vector_t{-1, 1, 0}, 0};
    co_yield {center + vector_t{-1, 1, 1}, 0};
    co_yield {center + vector_t{1, 0, -1}, 0};
    co_yield {center + vector_t{1, 0, 0}, 0};
    co_yield {center + vector_t{1, 0, 1}, 0};
    co_yield {center + vector_t{1, -1, -1}, 0};
    co_yield {center + vector_t{1, -1, 0}, 0};
    co_yield {center + vector_t{1, -1, 1}, 0};
    co_yield {center + vector_t{1, 1, -1}, 0};
    co_yield {center + vector_t{1, 1, 0}, 0};
    co_yield {center + vector_t{1, 1, 1}, 0};
  } else {
    const auto radius = old_radius / 2;
    co_yield {center + vector_t{radius, radius, radius}, radius};
    co_yield {center + vector_t{radius, radius, -radius}, radius};
    co_yield {center + vector_t{radius, -radius, radius}, radius};
    co_yield {center + vector_t{radius, -radius, -radius}, radius};
    co_yield {center + vector_t{-radius, radius, radius}, radius};
    co_yield {center + vector_t{-radius, radius, -radius}, radius};
    co_yield {center + vector_t{-radius, -radius, radius}, radius};
    co_yield {center + vector_t{-radius, -radius, -radius}, radius};
  }
}

using data_t = std::vector<sphere_t>;

struct d23 {
  static data_t convert(std::string_view input) {
    return data_t(
        std::from_range,
        aoc::lines(aoc::trimmed(input)) |
            std::views::transform([](std::string_view line) {
              static const auto re =
                  std::regex(R"(pos=<(-?\d+),(-?\d+),(-?\d+)>, r=(\d+))");
              auto match = *aoc::regex_match(line, re);
              return sphere_t{.pos = {*aoc::from_chars<value_t>(match.str(1)),
                                      *aoc::from_chars<value_t>(match.str(2)),
                                      *aoc::from_chars<value_t>(match.str(3))},
                              .r = *aoc::from_chars<value_t>(match.str(4))};
            }));
  }

  static size_t part1(const data_t &bots) {
    sphere_t strongest = std::ranges::max(bots, {}, &sphere_t::r);
    return std::ranges::count_if(bots, [&strongest](const sphere_t &n) {
      return aoc::manhattan_distance(strongest.pos, n.pos) <= strongest.r;
    });
  }

  static size_t part2(const data_t &bots) {
    value_t max_dist =
        std::ranges::max(bots | std::views::transform([](const auto &b) {
                           return aoc::manhattan_distance(b.pos, ORIGIN);
                         }));
    value_t starting_radius = 1;
    while (starting_radius < max_dist) {
      starting_radius *= 2;
    }
    auto to_search = std::set({search_space_t{
        .missing_bots = 0,
        .distance = 0,
        .cube = {.center = ORIGIN, .half_size = starting_radius}}});

    while (!to_search.empty()) {
      auto cur = *to_search.begin();
      to_search.erase(to_search.begin());
      if (cur.cube.half_size == 0) {
        return cur.distance;
      }
      for (oriented_cube_t splitted : split_search(cur.cube)) {
        auto new_space = search_space_t{
            .missing_bots = static_cast<size_t>(
                std::ranges::count_if(bots,
                                      [&splitted](const sphere_t &bot) {
                                        return !intersects(bot, splitted);
                                      })),
            .distance = aoc::manhattan_distance(splitted.center, ORIGIN),
            .cube = splitted};
        to_search.insert(new_space);
      }
    }
    throw std::runtime_error("No solution found");
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d23)

#else

#include <gtest/gtest.h>

TEST(d23, part1) {
  EXPECT_EQ(aoc::part1<d23>(aoc::arguments::make_example(R"(
pos=<0,0,0>, r=4
pos=<1,0,0>, r=1
pos=<4,0,0>, r=3
pos=<0,2,0>, r=1
pos=<0,5,0>, r=3
pos=<0,0,3>, r=1
pos=<1,1,1>, r=1
pos=<1,1,2>, r=1
pos=<1,3,1>, r=1
)")),
            7);
}

TEST(d23, part2) {
  EXPECT_EQ(aoc::part2<d23>(aoc::arguments::make_example(R"(
pos=<10,12,12>, r=2
pos=<12,14,12>, r=2
pos=<16,12,12>, r=4
pos=<14,14,14>, r=6
pos=<50,50,50>, r=200
pos=<10,10,10>, r=5
)")),
            36);
}

#endif
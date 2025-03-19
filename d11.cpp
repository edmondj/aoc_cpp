#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry.hpp>
#include <aoc_lib/string.hpp>

#include <format>

using data = aoc::dyn_matrix2d<int64_t>;

struct d11 {
  static data make_grid(std::string_view input) {
    data grid(300, 300);
    auto serial = *aoc::from_chars<int64_t>(aoc::trimmed(input));

    for (uint64_t y = 0; y < 300; ++y) {
      for (uint64_t x = 0; x < 300; ++x) {
        int64_t rack_id = x + 1 + 10;
        grid[{x, y}] = ((rack_id * (y + 1) + serial) * rack_id / 100) % 10 - 5;
      }
    }
    return grid;
  }

  static data convert(std::string_view input) {
    data result = make_grid(input);

    for (uint64_t y = 0; y < result.height(); ++y) {
      for (uint64_t x = 0; x < result.width(); ++x) {
        auto &local_sum = result[{x, y}];
        if (x > 0) {
          local_sum += result[{static_cast<size_t>(x - 1), y}];
        }
        if (y > 0) {
          local_sum += result[{x, static_cast<size_t>(y - 1)}];
        }
        if (x > 0 && y > 0) {
          local_sum -=
              result[{static_cast<size_t>(x - 1), static_cast<size_t>(y - 1)}];
        }
      }
    }

    return result;
  }

  static std::pair<int64_t, aoc::point2d<uint64_t>>
  find_max_square(const data &d, aoc::point2d<uint64_t> start,
                  uint64_t square_size, uint64_t search_size) {
    return std::ranges::max(
        // for each points
        std::views::iota(uint64_t(start.x),
                         uint64_t(start.x + square_size - search_size + 1)) |
            std::views::transform([start, square_size,
                                   search_size](uint64_t x) {
              return std::views::iota(
                         uint64_t(start.y),
                         uint64_t(start.y + square_size - search_size + 1)) |
                     std::views::transform([x](uint64_t y) {
                       return aoc::point2d<uint64_t>{x, y};
                     });
            }) |
            std::views::join |
            // map to pair of [SxS sum, point]
            std::views::transform([&d,
                                   search_size](aoc::point2d<uint64_t> point) {
              auto delta = static_cast<uint64_t>(search_size - 1);
              auto sum = d[point + aoc::vec2d{delta, delta}];
              if (point.x > 0) {
                sum -= d[{static_cast<size_t>(point.x - 1), point.y + delta}];
              }
              if (point.y > 0) {
                sum -= d[{point.x + delta, static_cast<size_t>(point.y - 1)}];
              }
              if (point.x > 0 && point.y > 0) {
                sum += d[{static_cast<size_t>(point.x - 1),
                          static_cast<size_t>(point.y - 1)}];
              }

              return std::make_pair(sum, point);
            }),
        {}, [](const auto &pair) { return pair.first; });
  }

  static std::string part1(const data &d) {
    auto [sum, top_left] = find_max_square(d, {0, 0}, 300, 3);
    uint64_t search_size = 3;
    aoc::point2d<uint64_t> point{20, 60};
    aoc::point2d<uint64_t> point2 =
        point + aoc::vec2d<uint64_t>(search_size - 1, search_size - 1);
    return std::format("{},{}", top_left.x + 1, top_left.y + 1);
  }

  static std::string part2(const data &d) {
    auto [sum, point, size] =
        std::ranges::max(std::views::iota(uint64_t(1), uint64_t(301)) |
                             std::views::transform([&d](uint64_t size) {
                               auto [sum, point] =
                                   find_max_square(d, {0, 0}, 300, size);
                               return std::make_tuple(sum, point, size);
                             }),
                         {}, [](const auto &t) { return std::get<0>(t); });
    return std::format("{},{},{}", point.x + 1, point.y + 1, size);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d11)

#else

#include <gtest/gtest.h>

TEST(d11, convert) {
  EXPECT_EQ(d11::make_grid("8").at({3 - 1, 5 - 1}), 4);
  EXPECT_EQ(d11::make_grid("57").at({122 - 1, 79 - 1}), -5);
  EXPECT_EQ(d11::make_grid("39").at({217 - 1, 196 - 1}), 0);
  EXPECT_EQ(d11::make_grid("71").at({101 - 1, 153 - 1}), 4);
}

TEST(d11, part1) {
  EXPECT_EQ(aoc::part1<d11>(aoc::arguments::make_example("42")), "21,61");
}

TEST(d11, part2) {
  EXPECT_EQ(aoc::part2<d11>(aoc::arguments::make_example("18")), "90,269,16");
  EXPECT_EQ(aoc::part2<d11>(aoc::arguments::make_example("42")), "232,251,12");
}

#endif
#pragma once

#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/geometry/point.hpp>

#include <optional>
#include <ranges>

namespace aoc {
template <std::ranges::input_range Points> auto centroid(Points &&points) {
  return aoc::mean(std::forward<Points>(points) |
                   std::views::transform([]<typename T>(T &&point) {
                     return std::forward<T>(point).matrix();
                   }))
      .transform([]<typename T>(T &&matrix) {
        return std::ranges::range_value_t<Points>(std::forward<T>(matrix));
      });
}

//   requires is_point<std::ranges::range_value_t<Points>>
// {
//   using Point = std::ranges::range_value_t<Points>;
//   point_value_t<Point> x = {}, y = {}, size = {};
//   for (const auto &point : points) {
//     x += point.x;
//     y += point.y;
//     size += 1;
//   }
//   if (size == 0) {
//     return std::nullopt;
//   }
//   return Point{.x = x / size, .y = y / size};
// }
} // namespace aoc
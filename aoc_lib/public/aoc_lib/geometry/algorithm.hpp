#pragma once

#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/geometry/point.hpp>
#include <aoc_lib/geometry/vector.hpp>

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

template <scalar S, size_t M>
S manhattan_distance(const vector<S, M> &vec)
  requires std::is_integral_v<S>
{
  using std::abs;
  return [&]<S... I>(std::integer_sequence<S, I...>) {
    return (0 + ... + abs(vec.at(I)));
  }(std::make_integer_sequence<S, M>());
}

} // namespace aoc
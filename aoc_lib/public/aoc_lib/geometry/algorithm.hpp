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

namespace adjacent_type {
struct manhattan_t {};
constexpr manhattan_t manhattan;

struct euclidean_t {};
constexpr euclidean_t euclidean;
} // namespace adjacent_type

template <scalar S>
auto adjacent(adjacent_type::euclidean_t, point2d<S> center)
  requires std::is_signed_v<S>
{
  static const vector2d<S> directions[] = {
      {S{0}, S{1}},  {S{1}, S{1}},   {S{1}, S{0}},  {S{1}, S{-1}},
      {S{0}, S{-1}}, {S{-1}, S{-1}}, {S{-1}, S{0}}, {S{-1}, S{1}}};

  return directions |
         std::views::transform([center = std::move(center)](vector2d<S> dir) {
           return center + dir;
         });
}
} // namespace aoc
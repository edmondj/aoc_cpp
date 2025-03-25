#pragma once

#include <algorithm>
#include <cmath>
#include <ranges>

namespace aoc {
template <std::ranges::input_range R>
constexpr std::optional<std::ranges::range_value_t<R>> sum(R &&r) {
  std::ranges::range_value_t<R> result{};
  for (auto &&v : r) {
    result += std::forward_like<R>(v);
  }
  return result;
}

template <std::ranges::input_range R>
constexpr std::optional<std::ranges::range_value_t<R>> mean(R &&r) {
  std::ranges::range_value_t<R> result{};
  std::size_t size{};
  for (auto &&v : r) {
    result += std::forward_like<R>(v);
    size += 1;
  }
  return result / size;
}

template <std::ranges::input_range Values>
auto rms(Values &&values) -> std::optional<std::ranges::range_value_t<Values>> {
  return mean(std::forward<Values>(values) |
              std::views::transform([](auto &&v) { return v * v; }))
      .transform([]<typename T>(T &&value) {
        using std::sqrt;
        return sqrt(std::forward<T>(value));
      });
}

} // namespace aoc
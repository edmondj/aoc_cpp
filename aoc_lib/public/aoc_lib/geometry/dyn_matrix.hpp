#pragma once

#include <aoc_lib/geometry/point.hpp>

#include <cassert>
#include <ranges>
#include <vector>

namespace aoc {

template <typename T> class dyn_matrix {
public:
  using point_t = point2d<size_t>;

  dyn_matrix() : m_width(0) {}

  dyn_matrix(size_t width, size_t height)
      : m_width(width), m_data(width * height) {}

  dyn_matrix(size_t width, size_t height, const T &init)
      : m_width(width), m_data(width * height, init) {}

  dyn_matrix(size_t width, size_t height, std::initializer_list<T> data)
      : m_width(width), m_data(data) {
    m_data.resize(width * height);
  }

  dyn_matrix(size_t width, size_t height, std::vector<T> data)
      : m_width(width), m_data(std::move(data)) {
    m_data.resize(width * height);
  }

  template <std::ranges::input_range R>
  dyn_matrix(size_t width, size_t height, R &&data)
    requires std::convertible_to<T, std::ranges::range_value_t<R>>
      : m_width(width), m_data(std::from_range, std::forward<R>(data)) {
    m_data.resize(width * height);
  }

  template <std::ranges::input_range R>
  dyn_matrix(std::from_range_t from_range, R &&lines)
    requires std::ranges::input_range<std::ranges::range_value_t<R>>
      : m_width(std::ranges::size(*lines.begin())),
        m_data(from_range, lines | std::views::join) {}

  constexpr bool contains(size_t m, size_t n) const {
    return n < width() && m < height();
  }

  constexpr bool contains(const point_t &p) const {
    return contains(p.y(), p.x());
  }

  template <typename Self>
  constexpr decltype(auto) operator[](this Self &&self, size_t m, size_t n) {
    return std::forward<Self>(self).at(m, n);
  }

  template <typename Self>
  constexpr decltype(auto) operator[](this Self &&self, const point_t &p) {
    return std::forward<Self>(self).at(p.y(), p.x());
  }

  template <typename Self>
  constexpr decltype(auto) at(this Self &&self, size_t m, size_t n) {
    return std::forward_like<Self>(self.m_data[m * self.m_width + n]);
  }

  template <typename Self>
  constexpr decltype(auto) at(this Self &&self, const point_t &p) {
    return std::forward<Self>(self).at(p.y(), p.x());
  }

  size_t width() const { return m_width; }
  size_t height() const { return m_width > 0 ? m_data.size() / width() : 0; }

  bool operator==(const dyn_matrix &r) const = default;

  std::ranges::view auto data_view() const { return m_data | std::views::all; }

  std::ranges::view auto enumerate() const {
    return m_data | std::views::enumerate |
           std::views::transform(
               [width = m_width](std::tuple<size_t, const T &> t)
                   -> std::tuple<point_t, const T &> {
                 size_t i = std::get<0>(t);
                 return {point2d{i % width, i / width}, std::get<1>(t)};
               });
  }

private:
  size_t m_width = 0;
  std::vector<T> m_data;
};

template <typename R>
dyn_matrix(std::from_range_t, R &&) -> dyn_matrix<std::remove_cvref_t<
    std::ranges::range_value_t<std::ranges::range_value_t<R>>>>;

} // namespace aoc
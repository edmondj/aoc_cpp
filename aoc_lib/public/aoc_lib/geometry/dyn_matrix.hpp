#pragma once

#include <aoc_lib/geometry/point.hpp>

#include <vector>

namespace aoc {

template <typename T> class dyn_matrix {
public:
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

  constexpr bool contains(size_t m, size_t n) const {
    return n < width() && m < height();
  }

  constexpr bool contains(const point2d<size_t> &p) const {
    return contains(p.y(), p.x());
  }

  template <typename Self>
  constexpr auto &&operator[](this Self &&self, size_t m, size_t n) {
    return std::forward<Self>(self).at(m, n);
  }

  template <typename Self>
  constexpr auto &&operator[](this Self &&self, const point2d<size_t> &p) {
    return std::forward<Self>(self).at(p.y(), p.x());
  }

  template <typename Self>
  constexpr auto &&at(this Self &&self, size_t m, size_t n) {
    return std::forward<Self>(self).m_data[m * self.m_width + n];
  }

  template <typename Self>
  constexpr auto &&at(this Self &&self, const point2d<size_t> &p) {
    return std::forward<Self>(self).at(p.y(), p.x());
  }

  size_t width() const { return m_width; }
  size_t height() const { return m_width > 0 ? m_data.size() / width() : 0; }

private:
  size_t m_width = 0;
  std::vector<T> m_data;
};

} // namespace aoc
#pragma once

#include <aoc_lib/geometry/fixed_matrix.hpp>

namespace aoc {
template <typename T, std::size_t M> class point {
public:
  using matrix_type = fixed_matrix<T, M, 1>;
  using value_type = matrix_type::value_type;

  constexpr point() = default;
  constexpr point(const point &) = default;
  constexpr point(point &&) = default;
  constexpr point(std::initializer_list<T> init) : m_matrix(init) {}
  constexpr explicit point(const matrix_type &values) : m_matrix(values) {}
  constexpr explicit point(matrix_type &&values)
      : m_matrix(std::move(values)) {}

  constexpr point &operator=(const point &) = default;
  constexpr point &operator=(point &&) = default;

  constexpr bool operator==(const point &) const = default;

  template <typename Self> constexpr auto &&matrix(this Self &&self) {
    return std::forward_like<Self>(self.m_matrix);
  }

  template <typename Self>
  constexpr auto &&at(this Self &&self, std::size_t m) {
    return std::forward<Self>(self).matrix().at(m, 0);
  }
  template <typename Self>
  constexpr auto &&operator[](this Self &&self, std::size_t m) {
    return std::forward<Self>(self).at(m);
  }

  template <typename Self>
  constexpr auto &&x(this Self &&self)
    requires(dimensions<std::remove_cvref_t<Self>>::M() >= 1 &&
             dimensions<std::remove_cvref_t<Self>>::M() <= 3)
  {
    return std::forward<Self>(self).at(0);
  }

  template <typename Self>
  constexpr auto &&y(this Self &&self)
    requires(dimensions<std::remove_cvref_t<Self>>::M() >= 2 &&
             dimensions<std::remove_cvref_t<Self>>::M() <= 3)
  {
    return std::forward<Self>(self).at(1);
  }

  template <typename Self>
  constexpr auto &&z(this Self &&self)
    requires(dimensions<std::remove_cvref_t<Self>>::M() == 3)
  {
    return std::forward<Self>(self).at(2);
  }

private:
  matrix_type m_matrix;
};

template <typename T> using point2d = point<T, 2>;

template <typename T, std::size_t M_> struct dimensions<point<T, M_>> {
  static constexpr std::size_t M() { return M_; }
};

template <typename T, std::size_t M> struct std::hash<point<T, M>> {
  size_t operator()(const aoc::point<T, M> &p) const noexcept {
    return std::hash<aoc::point<T, M>::matrix_type>{}(p.matrix());
  }
};

} // namespace aoc
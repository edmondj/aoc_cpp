#pragma once

#include <aoc_lib/geometry/fixed_matrix.hpp>

#include <ranges>

namespace aoc {
template <scalar T, std::size_t M> class point {
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

  template <scalar U>
  constexpr operator point<U, M>() const
    requires std::convertible_to<T, U>
  {
    return point<U, M>(m_matrix);
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
template <typename T> using point3d = point<T, 3>;

template <scalar S, size_t M>
S manhattan_distance(const point<S, M> &l, const point<S, M> &r) {
  return [&l, &r]<size_t... I>(std::index_sequence<I...>) {
    using std::abs;
    return (S{} + ... + abs(r[I] - l[I]));
  }(std::make_index_sequence<M>());
}

namespace views {
template <scalar S> auto point2d_iota(S m, S M, S n, S N) {
  return std::views::iota(m, m + M) | std::views::transform([n, N](S m) {
           return std::views::iota(n, n + N) |
                  std::views::transform([m](S n) { return point2d{m, n}; });
         }) |
         std::views::join;
}

template <scalar S> auto point2d_iota(S M, S N) {
  return point2d_iota(S{}, M, S{}, N);
}

} // namespace views

} // namespace aoc
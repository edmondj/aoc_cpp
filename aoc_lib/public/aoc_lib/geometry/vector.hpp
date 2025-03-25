#pragma once

#include <ranges>

#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/geometry/fixed_matrix.hpp>
#include <aoc_lib/geometry/point.hpp>

namespace aoc {
template <typename T, std::size_t M> class vector {
public:
  using matrix_type = fixed_matrix<T, M, 1>;
  using value_type = matrix_type::value_type;

  constexpr vector() = default;
  constexpr vector(const vector &) = default;
  constexpr vector(vector &&) = default;
  constexpr vector(std::initializer_list<T> init) : m_matrix(init) {}
  constexpr explicit vector(const matrix_type &values) : m_matrix(values) {}
  constexpr explicit vector(matrix_type &&values)
      : m_matrix(std::move(values)) {}

  constexpr vector &operator=(const vector &) = default;
  constexpr vector &operator=(vector &&) = default;

  constexpr bool operator==(const vector &) const = default;

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
  constexpr auto &&dx(this Self &&self)
    requires(dimensions<std::remove_cvref_t<Self>>::M() >= 1 &&
             dimensions<std::remove_cvref_t<Self>>::M() <= 3)
  {
    return std::forward<Self>(self).at(0);
  }

  template <typename Self>
  constexpr auto &&dy(this Self &&self)
    requires(dimensions<std::remove_cvref_t<Self>>::M() >= 2 &&
             dimensions<std::remove_cvref_t<Self>>::M() <= 3)
  {
    return std::forward<Self>(self).at(1);
  }

  template <typename Self>
  constexpr auto &&dz(this Self &&self)
    requires(dimensions<std::remove_cvref_t<Self>>::M() == 3)
  {
    return std::forward<Self>(self).at(2);
  }

  template <typename U>
  friend constexpr auto operator*(const vector &l, const U &r) {
    auto values = l.matrix() * r;
    return vector<decltype(values)::value_type, M>(std::move(values));
  }

  template <typename U>
  friend constexpr auto operator*(const U &l, const vector &r) {
    auto values = l * r.matrix();
    return vector<decltype(values)::value_type, M>(std::move(values));
  }

  template <typename U>
  friend constexpr auto operator+(const vector &l, const point<U, M> &r) {
    auto values = l.matrix() + r.matrix();
    return point<decltype(values)::value_type, M>(std::move(values));
  }

  template <typename U>
  friend constexpr auto operator+(const point<U, M> &l, const vector &r) {
    auto values = l.matrix() + r.matrix();
    return point<decltype(values)::value_type, M>(std::move(values));
  }

  template <typename U>
  friend constexpr auto operator-(const vector &l, const point<U, M> &r) {
    auto values = l.matrix() - r.matrix();
    return point<decltype(values)::value_type, M>(std::move(values));
  }

  template <typename U>
  friend constexpr auto operator-(const point<U, M> &l, const vector &r) {
    auto values = l.matrix() - r.matrix();
    return point<decltype(values)::value_type, M>(std::move(values));
  }

private:
  matrix_type m_matrix;
};

template <typename T> using vector2d = vector<T, 2>;

template <typename T, std::size_t M_> struct dimensions<vector<T, M_>> {
  static constexpr std::size_t M() { return M_; }
};

template <typename T, std::size_t M> struct std::hash<vector<T, M>> {
  size_t operator()(const aoc::vector<T, M> &v) const noexcept {
    return std::hash<aoc::vector<T, M>::matrix_type>{}(v.matrix());
  }
};

template <typename T, std::size_t M, typename U>
auto operator-(const point<T, M> &l, const point<U, M> &r) {
  auto values = l.matrix() - r.matrix();
  return vector<decltype(values)::value_type, M>(std::move(values));
}

template <typename T, std::size_t M, typename U>
point<T, M> &operator-=(point<T, M> &l, const vector<U, M> &r) {
  l.matrix() -= r.matrix();
  return l;
}

template <typename T, std::size_t M>
constexpr auto amplitude(const vector<T, M> &v)
  requires(M > 0)
{
  return aoc::sum(std::views::iota(std::size_t{}, M) |
                  std::views::transform(
                      [&v](size_t n) { return v.at(n) * v.at(n); }))
      .transform([]<typename T>(T &&value) {
        using std::sqrt;
        return sqrt(std::forward<T>(value));
      })
      .value();
}

} // namespace aoc
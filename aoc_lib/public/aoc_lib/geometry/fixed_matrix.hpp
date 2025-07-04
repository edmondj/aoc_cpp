#pragma once

#include <aoc_lib/geometry/dimensions.hpp>
#include <aoc_lib/geometry/scalar.hpp>
#include <aoc_lib/hash.hpp>

#include <algorithm>
#include <array>
#include <ranges>

namespace aoc {
template <scalar T, std::size_t M, std::size_t N> class fixed_matrix {
public:
  using array_type = std::array<T, M * N>;
  using value_type = T;

  constexpr fixed_matrix() = default;
  constexpr fixed_matrix(const fixed_matrix &) = default;
  constexpr fixed_matrix(fixed_matrix &&) = default;
  template <std::ranges::sized_range R>
  constexpr fixed_matrix(std::from_range_t, R &&r) {
    std::ranges::copy_n(std::ranges::begin(r),
                        std::min(std::ranges::size(r), m_data.size()),
                        m_data.begin());
  }
  constexpr fixed_matrix(std::initializer_list<T> init)
      : fixed_matrix(std::from_range, init) {}
  constexpr fixed_matrix(const array_type &values) : m_data(values) {}
  constexpr fixed_matrix(array_type &&values) : m_data(std::move(values)) {}

  constexpr fixed_matrix &operator=(const fixed_matrix &) = default;
  constexpr fixed_matrix &operator=(fixed_matrix &&) = default;

  constexpr bool operator==(const fixed_matrix &) const = default;

  const array_type &data() const { return m_data; }

  template <typename F>
  constexpr auto transform(F &&func) const
      -> fixed_matrix<std::invoke_result_t<F, const T &>, M, N> {
    using U = std::invoke_result_t<F, const T &>;
    typename fixed_matrix<U, M, N>::array_type result;
    std::ranges::transform(m_data, result.begin(), std::forward<F>(func));
    return fixed_matrix<U, M, N>(std::move(result));
  }

  template <scalar U>
  constexpr operator fixed_matrix<U, M, N>() const
    requires std::convertible_to<T, U>
  {
    return transform([](const T &v) -> U { return v; });
  }

  template <typename Self>
  constexpr auto &&at(this Self &&self, std::size_t m, std::size_t n) {
    return std::forward_like<Self>(self.m_data[m * N + n]);
  }

  template <typename Self>
  constexpr auto &&operator[](this Self &&self, std::size_t m, std::size_t n) {
    return std::forward<Self>(self).at(m, n);
  }

  static constexpr fixed_matrix identity()
    requires(M == N && std::convertible_to<int, T>)
  {
    fixed_matrix result{};
    for (size_t i = 0; i < M; ++i) {
      result.at(i, i) = 1;
    }
    return result;
  }

  template <scalar U>
  friend constexpr auto operator*(const fixed_matrix &l, const U &r) {
    return l.transform([&r](const T &l) { return l * r; });
  }

  template <scalar U>
  friend constexpr auto operator*(const U &l, const fixed_matrix &r) {
    return r.transform([&l](const T &r) { return l * r; });
  }

  template <scalar U>
  friend constexpr auto operator/(const fixed_matrix &l, const U &r) {
    return l.transform([&r](const T &l) { return l / r; });
  }

  template <scalar U>
  friend constexpr auto operator+(const fixed_matrix &l,
                                  const fixed_matrix<U, M, N> &r) {
    auto range = std::views::zip(l.data(), r.data()) |
                 std::views::transform([]<typename Tuple>(Tuple &&t) {
                   return std::get<0>(std::forward<Tuple>(t)) +
                          std::get<1>(std::forward<Tuple>(t));
                 });
    return fixed_matrix<std::ranges::range_value_t<decltype(range)>, M, N>(
        std::from_range, std::move(range));
  }

  template <scalar U>
  friend constexpr auto operator-(const fixed_matrix &l,
                                  const fixed_matrix<U, M, N> &r) {
    auto range = std::views::zip(l.data(), r.data()) |
                 std::views::transform([]<typename Tuple>(Tuple &&t) {
                   return std::get<0>(std::forward<Tuple>(t)) -
                          std::get<1>(std::forward<Tuple>(t));
                 });
    return fixed_matrix<std::ranges::range_value_t<decltype(range)>, M, N>(
        std::from_range, std::move(range));
  }

  template <scalar U>
  friend constexpr fixed_matrix operator+=(fixed_matrix &l,
                                           const fixed_matrix<U, M, N> &r) {
    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        l.at(m, n) += r.at(m, n);
      }
    }
    return l;
  }

  template <scalar U>
  friend constexpr fixed_matrix operator-=(fixed_matrix &l,
                                           const fixed_matrix<U, M, N> &r) {
    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        l.at(m, n) -= r.at(m, n);
      }
    }
    return l;
  }

  template <scalar U, std::size_t P>
  friend constexpr auto operator*(const fixed_matrix &l,
                                  const fixed_matrix<U, N, P> &r) {
    using MulRes = decltype(std::declval<T>() * std::declval<U>());
    using Res = decltype(std::declval<MulRes>() + std::declval<MulRes>());
    fixed_matrix<Res, M, P> result{};

    for (size_t i = 0; i < M; ++i) {
      for (size_t j = 0; j < P; ++j) {
        for (size_t k = 0; k < N; ++k) {
          result.at(i, j) += l.at(i, k) * r.at(k, j);
        }
      }
    }

    return result;
  }

private:
  array_type m_data;
};

template <typename T> using matrix2d = fixed_matrix<T, 2, 2>;

template <typename T, std::size_t M_, std::size_t N_>
struct dimensions<fixed_matrix<T, M_, N_>> {
  static constexpr std::size_t M() { return M_; }
  static constexpr std::size_t N() { return N_; }
};

template <typename T, std::size_t M, std::size_t N>
struct std::hash<fixed_matrix<T, M, N>> {
  size_t operator()(const aoc::fixed_matrix<T, M, N> &p) const noexcept {
    std::hash<T> hasher;
    aoc::hash_accumulator acc;

    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        acc.accumulate(hasher(p.at(m, n)));
      }
    }
    return acc.result();
  }
};

} // namespace aoc
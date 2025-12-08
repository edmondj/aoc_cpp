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
  template <std::ranges::range R>
  constexpr fixed_matrix(std::from_range_t, R &&r) : m_data{} {
    std::ranges::copy(std::forward<R>(r) | std::views::take(m_data.size()),
                      m_data.begin());
  }
  constexpr fixed_matrix(std::initializer_list<T> init)
      : fixed_matrix(std::from_range, init) {}
  constexpr fixed_matrix(const array_type &values) : m_data(values) {}
  constexpr fixed_matrix(array_type &&values) : m_data(std::move(values)) {}

  constexpr fixed_matrix &operator=(const fixed_matrix &) = default;
  constexpr fixed_matrix &operator=(fixed_matrix &&) = default;

  constexpr auto operator<=>(const fixed_matrix &) const = default;

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

template <typename T> constexpr bool is_fixed_matrix = false;

template <scalar T, std::size_t M, std::size_t N>
constexpr bool is_fixed_matrix<fixed_matrix<T, M, N>> = true;

template <typename T>
concept matrix_convertible =
    is_fixed_matrix<typename T::matrix_type> && requires(T t) {
      { t.matrix() } -> std::convertible_to<typename T::matrix_type>;
    };

template <typename T, std::size_t M_, std::size_t N_>
struct dimensions<fixed_matrix<T, M_, N_>> {
  static constexpr std::size_t M() { return M_; }
  static constexpr std::size_t N() { return N_; }
};

template <matrix_convertible T>
struct dimensions<T> : dimensions<typename T::matrix_type> {};

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

template <matrix_convertible T>
struct std::hash<T> : std::hash<typename T::matrix_type> {
  size_t operator()(const T &t) const noexcept {
    return std::hash<typename T::matrix_type>::operator()(t.matrix());
  }
};

template <scalar S, size_t M, size_t N>
struct std::tuple_size<fixed_matrix<S, M, N>>
    : std::integral_constant<size_t, M * N> {};

template <size_t I, scalar S, size_t M, size_t N>
struct std::tuple_element<I, fixed_matrix<S, M, N>> {
  using type = S;
};

template <matrix_convertible T>
struct std::tuple_size<T> : std::tuple_size<typename T::matrix_type> {};

template <size_t I, matrix_convertible T>
struct std::tuple_element<I, T>
    : std::tuple_element<I, typename T::matrix_type> {};

template <size_t I, typename Self>
  requires(is_fixed_matrix<Self> && I < std::tuple_size_v<Self>)
constexpr auto &&get(Self &&self) {
  using dim = dimensions<std::remove_cvref_t<Self>>;
  return std::forward<Self>(self).at(I / dim::N(), I % dim::N());
}

template <size_t I, matrix_convertible Self> constexpr auto &&get(Self &&self) {
  return get<I>(std::forward<Self>(self).matrix());
}
} // namespace aoc
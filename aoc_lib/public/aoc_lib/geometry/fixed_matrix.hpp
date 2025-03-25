#pragma once

#include <aoc_lib/geometry/dimensions.hpp>

#include <algorithm>
#include <array>

namespace aoc {
template <typename T, std::size_t M, std::size_t N> class fixed_matrix {
public:
  using array_type = std::array<T, M * N>;
  using value_type = T;

  constexpr fixed_matrix() = default;
  constexpr fixed_matrix(const fixed_matrix &) = default;
  constexpr fixed_matrix(fixed_matrix &&) = default;
  constexpr fixed_matrix(std::initializer_list<T> init) {
    std::ranges::copy_n(init.begin(), std::min(init.size(), m_values.size()),
                        m_values.begin());
  }
  constexpr fixed_matrix(const array_type &values) : m_values(values) {}
  constexpr fixed_matrix(array_type &&values) : m_values(std::move(values)) {}

  constexpr fixed_matrix &operator=(const fixed_matrix &) = default;
  constexpr fixed_matrix &operator=(fixed_matrix &&) = default;

  constexpr bool operator==(const fixed_matrix &) const = default;

  template <typename F>
  constexpr auto transform(F &&func) const
      -> fixed_matrix<std::invoke_result_t<F, const T &>, M, N> {
    using U = std::invoke_result_t<F, const T &>;
    typename fixed_matrix<U, M, N>::array_type result;
    std::ranges::transform(m_values, result.begin(), std::forward<F>(func));
    return fixed_matrix<U, M, N>(std::move(result));
  }

  template <typename U>
  constexpr operator fixed_matrix<U, M, N>() const
    requires std::convertible_to<T, U>
  {
    return transform([](const T &v) -> U { return v; });
  }

  template <typename Self>
  constexpr auto &&at(this Self &&self, std::size_t m, std::size_t n) {
    return std::forward_like<Self>(self.m_values[m * N + n]);
  }

  template <typename Self>
  constexpr auto &&operator[](this Self &&self, std::size_t m, std::size_t n) {
    return std::forward<Self>(self).at(m, n);
  }

  template <typename U>
  friend constexpr auto operator*(const fixed_matrix &l, const U &r) {
    using Res = decltype(l.at(0, 0) * r);
    fixed_matrix<Res, M, N> result;

    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        result.at(m, n) = l.at(m, n) * r;
      }
    }

    return result;
  }

  template <typename U>
  friend constexpr auto operator/(const fixed_matrix &l, const U &r) {
    using Res = decltype(l.at(0, 0) / r);
    fixed_matrix<Res, M, N> result;

    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        result.at(m, n) = l.at(m, n) / r;
      }
    }

    return result;
  }
  template <typename U>
  friend constexpr auto operator*(const U &l, const fixed_matrix &r) {
    using Res = decltype(l * r.at(0, 0));
    fixed_matrix<Res, M, N> result;

    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        result.at(m, n) = l * r.at(m, n);
      }
    }

    return result;
  }

  template <typename U>
  friend constexpr auto operator+(const fixed_matrix &l,
                                  const fixed_matrix<U, M, N> &r) {
    using Res = decltype(l.at(0, 0) + r.at(0, 0));
    fixed_matrix<Res, M, N> result;

    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        result.at(m, n) = l.at(m, n) + r.at(m, n);
      }
    }

    return result;
  }

  template <typename U>
  friend constexpr fixed_matrix operator+=(fixed_matrix &l,
                                           const fixed_matrix<U, M, N> &r) {
    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        l.at(m, n) += r.at(m, n);
      }
    }
    return l;
  }

  template <typename U>
  friend constexpr auto operator-(const fixed_matrix &l,
                                  const fixed_matrix<U, M, N> &r) {
    using Res = decltype(l.at(0, 0) - r.at(0, 0));
    fixed_matrix<Res, M, N> result;

    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        result.at(m, n) = l.at(m, n) - r.at(m, n);
      }
    }

    return result;
  }

  template <typename U>
  friend constexpr fixed_matrix operator-=(fixed_matrix &l,
                                           const fixed_matrix<U, M, N> &r) {
    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        l.at(m, n) -= r.at(m, n);
      }
    }
    return l;
  }

private:
  array_type m_values;
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
    size_t hash = 0;

    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        hash ^= hasher(p.at(m, n)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
      }
    }
    return hash;
  }
};

} // namespace aoc
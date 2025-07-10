#pragma once

#include <aoc_lib/geometry/fixed_matrix.hpp>

#include <format>

template <typename S, std::size_t M, typename Char>
struct std::formatter<aoc::fixed_matrix<S, M, 1>, Char> {
  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext &ctx) noexcept {
    return m_member_formatter.parse(ctx);
  }

  template <typename FmtContext>
  FmtContext::iterator format(const aoc::fixed_matrix<S, M, 1> &m,
                              FmtContext &ctx) const {
    for (size_t i = 0; i < M; ++i) {
      if (i != 0) {
        ctx.advance_to(std::format_to(ctx.out(), ", "));
      }
      ctx.advance_to(m_member_formatter.format(m.at(i, 0), ctx));
    }
    return ctx.out();
  }

  std::formatter<S, Char> m_member_formatter;
};

template <aoc::matrix_convertible T, typename Char>
class std::formatter<T, Char>
    : public std::formatter<typename T::matrix_type, Char> {
public:
  template <typename FmtContext>
  FmtContext::iterator format(const T &t, FmtContext &ctx) const {
    return std::formatter<typename T::matrix_type, Char>::format(t.matrix(),
                                                                 ctx);
  }
};
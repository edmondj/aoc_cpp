#pragma once

#include <aoc_lib/geometry/dyn_matrix.hpp>

#include <format>

template <> struct std::formatter<aoc::dyn_matrix<char>, char> {
  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext &ctx) noexcept {
    return ctx.begin();
  }

  template <typename FmtContext>
  FmtContext::iterator format(const aoc::dyn_matrix<char> &m,
                              FmtContext &ctx) const {
    auto out = ctx.out();
    for (size_t y = 0; y < m.height(); ++y) {
      for (size_t x = 0; x < m.width(); ++x) {
        *(out++) = m.at(y, x);
      }
      *(out++) = '\n';
    }
    return ctx.out();
  }
};

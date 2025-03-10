#pragma once

#include <utility>

namespace aoc {
template <typename... TFuncs> struct overload : TFuncs... {
  overload(TFuncs &&...funcs) : TFuncs(std::forward<TFuncs>(funcs))... {}
  using TFuncs::operator()...;
};
} // namespace aoc
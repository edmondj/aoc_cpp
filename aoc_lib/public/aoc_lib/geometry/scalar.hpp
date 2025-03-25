#pragma once

#include <type_traits>

namespace aoc {
template <typename T>
concept scalar = std::is_scalar_v<T>;
}
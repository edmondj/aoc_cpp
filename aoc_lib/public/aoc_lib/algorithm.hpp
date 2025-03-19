#include <algorithm>
#include <ranges>

namespace aoc {
template <std::ranges::input_range R>
constexpr auto sum(R &&r, std::ranges::range_value_t<R> init = {}) {
  return std::ranges::fold_left(std::forward<R>(r), init, std::plus<>{});
}
} // namespace aoc
#include <algorithm>

namespace aoc {
template <typename Sum, std::ranges::input_range R>
constexpr Sum sum(R &&r, Sum init = {}) {
  return std::ranges::fold_left(std::forward<R>(r), init, std::plus<>{});
}
} // namespace aoc
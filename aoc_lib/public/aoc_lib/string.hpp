#pragma once

#include <algorithm>
#include <charconv>
#include <expected>
#include <ranges>
#include <string_view>

namespace aoc {
std::string_view trimmed(std::string_view src);

inline auto lines(std::string_view src) {
  return std::views::split(src, '\n') |
         std::views::transform([](auto subrange) {
           auto line = std::string_view(subrange);
           if (line.ends_with('\r')) {
             line.remove_suffix('\r');
           }
           return line;
         });
}

template <typename T>
std::expected<T, std::errc> from_chars(std::string_view chars, int base = 10) {
  T res;
  auto conv_result =
      std::from_chars(chars.data(), chars.data() + chars.size(), res, base);
  if (conv_result.ec != std::errc{}) {
    return std::unexpected(conv_result.ec);
  }
  return res;
}

} // namespace aoc

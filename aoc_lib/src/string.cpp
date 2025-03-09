#include "aoc_lib/string.hpp"

namespace aoc {

namespace {
constexpr std::string_view whitespaces = " \t\r\n";
}

std::string_view trimmed(std::string_view src) {
  auto start = src.find_first_not_of(whitespaces);
  auto end = src.find_last_not_of(whitespaces);

  return src.substr(start, end - start + 1);
}
} // namespace aoc
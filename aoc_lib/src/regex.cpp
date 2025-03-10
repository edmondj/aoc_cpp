#include "aoc_lib/regex.hpp"

namespace aoc {
std::optional<svmatch> regex_match(std::string_view str,
                                   const std::regex &regex) {
  svmatch match;
  if (!std::regex_match(str.begin(), str.end(), match, regex)) {
    return std::nullopt;
  }
  return match;
}

} // namespace aoc
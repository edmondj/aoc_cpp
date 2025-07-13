#pragma once

#include <optional>
#include <regex>
#include <string_view>

namespace aoc {
using svmatch = std::match_results<std::string_view::const_iterator>;

std::optional<svmatch> regex_match(std::string_view str,
                                   const std::regex &regex,
                                   std::regex_constants::match_flag_type flags =
                                       std::regex_constants::match_default);
} // namespace aoc
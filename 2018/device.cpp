#include "device.hpp"

#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

namespace device {

instruction_t parse_instruction(std::string_view s) {

  static const auto re = std::regex(R"(^(\w{4}) (\d+) (\d+) (\d+)$)");

  auto match = *aoc::regex_match(s, re);

  for (const auto &[opcode, label] : OPCODE_LABELS) {
    if (match.str(1) == label) {
      return instruction_t{.opcode = opcode,
                           .args = {*aoc::from_chars<value_t>(match.str(2)),
                                    *aoc::from_chars<value_t>(match.str(3)),
                                    *aoc::from_chars<value_t>(match.str(4))}};
    }
  }

  throw std::runtime_error("Unknown opcode");
}

program_t parse_program(std::string_view s) {
  static const auto ip_regex = std::regex(R"(^#ip (\d)$)");
  std::optional<size_t> ip;

  auto lines = aoc::lines(aoc::trimmed(s));
  auto first = lines.begin();
  if (auto match = aoc::regex_match(*first, ip_regex)) {
    ip = *aoc::from_chars<size_t>(match->str(1));
    ++first;
  }
  return program_t{
      .ip = ip,
      .instructions = std::vector{
          std::from_range, std::ranges::subrange(first, lines.end()) |
                               std::views::transform(&parse_instruction)}};
}

std::string_view label_for(opcode_t op) {
  for (const auto &[o, label] : OPCODE_LABELS) {
    if (op == o) {
      return label;
    }
  }
  std::unreachable();
}
} // namespace device

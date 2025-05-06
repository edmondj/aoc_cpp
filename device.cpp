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

std::string_view label_for(opcode_t op) {
  for (const auto &[o, label] : OPCODE_LABELS) {
    if (op == o) {
      return label;
    }
  }
  std::unreachable();
}
} // namespace device

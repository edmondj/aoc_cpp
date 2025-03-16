#pragma once

#include <optional>
#include <string>

namespace aoc {

enum class part { one, two };

struct arguments {
  std::string input;
  bool is_example = false;
  std::optional<part> selected_part;
  std::optional<std::string> expected_output;

  operator const std::string &() const { return input; }
  operator std::string_view() const { return input; }
};

arguments parse_arguments(int argc, const char **argv,
                          const char *app_name = nullptr);
} // namespace aoc
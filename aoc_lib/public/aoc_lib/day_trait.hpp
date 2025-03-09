#pragma once

#include <aoc_lib/input.hpp>
#include <format>

namespace aoc {

template <typename Trait>
auto convert(const aoc::arguments &args) -> decltype(auto) {
  if constexpr (requires { Trait::convert(args); }) {
    return Trait::convert(args);
  } else {
    return args;
  }
}

template <typename T>
concept day_trait =
    requires(const aoc::arguments &args) { T::part1(convert<T>(args)); };

template <typename T>
concept day_with_part2 = day_trait<T> && requires(const aoc::arguments &args) {
  T::part2(convert<T>(args));
};

template <day_trait Trait>
void execute_day(const aoc::arguments &args,
                 std::output_iterator<const char &> auto out) {
  decltype(auto) input = convert<Trait>(args);
  if (args.selected_part) {
    switch (*args.selected_part) {
    case part::one:
      std::format_to(out, "{}", Trait::part1(input));
      break;
    case part::two:
      if constexpr (day_with_part2<Trait>) {
        std::format_to(out, "{}", Trait::part2(input));
        break;
      } else {
        throw std::runtime_error("Part 2 not implemented");
      }
    }
  } else {
    if constexpr (day_with_part2<Trait>) {
      std::format_to(out, "Part 1:\n{}\nPart 2:\n{}\n", Trait::part1(input),
                     Trait::part2(input));
    } else {
      std::format_to(out, "{}", Trait::part1(input));
    }
  }
}

} // namespace aoc
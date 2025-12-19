#pragma once

#include <aoc_lib/input.hpp>

#include <chrono>
#include <format>

namespace aoc {

template <typename Trait>
auto convert(const aoc::arguments &args) -> const aoc::arguments & {
  return args;
}

template <typename Trait>
auto convert(const aoc::arguments &args) -> decltype(auto)
  requires requires { Trait::convert(args); }
{
  return Trait::convert(args);
}

template <typename Trait>
using converted_input =
    decltype(convert<Trait>(std::declval<const aoc::arguments &>()));

template <typename T>
concept day_with_part1 =
    requires(const aoc::arguments &args) { T::part1(convert<T>(args)); };

template <typename T>
concept day_with_part2 =
    day_with_part1<T> &&
    requires(const aoc::arguments &args) { T::part2(convert<T>(args)); };

template <typename T>
concept day_with_run =
    requires(const aoc::arguments &args) { T::run(convert<T>(args)); };

template <typename T>
concept day_trait = day_with_part1<T> || day_with_run<T>;

template <typename T>
concept day_run_result = requires(T t) {
  get<0>(t);
  get<1>(t);
};

template <day_with_part1 Trait> auto part1(converted_input<Trait> input) {
  return Trait::part1(input);
}

template <day_with_run Trait> auto part1(converted_input<Trait> input) {
  auto res = Trait::run(input);
  static_assert(day_run_result<decltype(res)>,
                "Result of run must be a tuple like type of size >= 2");
  return get<0>(res);
}

template <typename Trait>
auto part1(const aoc::arguments &args)
  requires requires { part1<Trait>(Trait::convert(args)); }
{
  return part1<Trait>(Trait::convert(args));
}

template <day_with_part2 Trait> auto part2(converted_input<Trait> input) {
  return Trait::part2(input);
}

template <day_with_run Trait> auto part2(converted_input<Trait> input) {
  auto res = Trait::run(input);
  static_assert(day_run_result<decltype(res)>,
                "Result of run must be a tuple like type of size >= 2");
  return get<1>(res);
}

template <typename Trait>
auto part2(const aoc::arguments &args)
  requires requires { part2<Trait>(Trait::convert(args)); }
{
  return part2<Trait>(Trait::convert(args));
}

void print_part(std::output_iterator<const char &> auto out,
                uint8_t part_number, auto &&value) {
  std::string_view separator = " ";
  if constexpr (requires { std::string_view(value); }) {
    if (std::string_view(value).contains('\n')) {
      separator = "\n";
    }
  }
  std::format_to(out, "Part {}:{}{}\n", part_number, separator, value);
}

template <day_trait Trait>
void execute_day(const aoc::arguments &args,
                 std::output_iterator<const char &> auto out) {
  auto start = std::chrono::steady_clock::now();
  if (args.selected_part) {
    switch (*args.selected_part) {
    case part::one:
      print_part(out, 1, part1<Trait>(args));
      break;
    case part::two:
      if constexpr (requires { part2<Trait>(args); }) {
        print_part(out, 2, part2<Trait>(args));
        break;
      } else {
        throw std::runtime_error("Part 2 not implemented");
      }
    }
  } else {
    decltype(auto) input = convert<Trait>(args);
    if constexpr (day_with_run<Trait>) {
      auto [part1, part2] = Trait::run(input);
      print_part(out, 1, part1);
      print_part(out, 2, part2);
    } else {
      print_part(out, 1, part1<Trait>(input));
      if constexpr (day_with_part2<Trait>) {
        print_part(out, 2, part2<Trait>(input));
      }
    }
  }
  auto end = std::chrono::steady_clock::now();
  std::format_to(
      out, "Execution time: {}ms",
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count());
}

} // namespace aoc
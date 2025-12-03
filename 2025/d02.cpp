#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <string>
#include <unordered_set>
#include <vector>

struct id_range {
  size_t from;
  size_t to;

  bool includes(size_t v) const { return from <= v && v <= to; }
};

struct d02 {

  static auto convert(std::string_view input) {
    return std::vector{
        std::from_range,
        aoc::split(aoc::trimmed(input), ',') |
            std::views::transform([](std::string_view range) {
              static const auto re = std::regex(R"((\d+)-(\d+))");
              const auto match = aoc::regex_match(range, re).value();
              return id_range{
                  .from = aoc::from_chars<size_t>(match[1].str()).value(),
                  .to = aoc::from_chars<size_t>(match[2].str()).value()};
            })};
  }

  static auto run(const auto &input) {
    auto max = std::ranges::max(input | std::views::transform(&id_range::to));
    auto part1 = std::unordered_set<size_t>{};
    auto part2 = std::unordered_set<size_t>{};
    auto magnitude = 10uz;
    for (size_t i = 1; true; ++i) {
      while (magnitude <= i) {
        magnitude *= 10;
      }
      auto val = i * magnitude + i;
      if (val > max) {
        break;
      }
      for (auto repeat = 2uz; val <= max; ++repeat) {
        if (std::ranges::any_of(input, [val](const auto &range) {
              return range.includes(val);
            })) {
          if (repeat == 2) {
            part1.insert(val);
          }
          part2.insert(val);
        }
        val = val * magnitude + i;
      }
    }
    return std::make_pair(aoc::sum(part1).value(), aoc::sum(part2).value());
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d02)

#else

#include <gtest/gtest.h>

const auto TEST_DATA = aoc::arguments::make_example(
    R"(11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124)");

TEST(d02, part1) { EXPECT_EQ(aoc::part1<d02>(TEST_DATA), 1227775554); }
TEST(d02, part2) { EXPECT_EQ(aoc::part2<d02>(TEST_DATA), 4174379265); }

#endif
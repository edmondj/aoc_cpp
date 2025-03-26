#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/string.hpp>

#include <array>
#include <vector>

struct d14 {

  static std::pair<std::string, size_t> run(std::string_view input) {
    size_t count = *aoc::from_chars<size_t>(aoc::trimmed(input));
    const auto target = std::vector<uint8_t>(
        std::from_range,
        aoc::trimmed(input) |
            std::views::transform([](char c) { return c - '0'; }));
    auto target_it = target.begin();

    auto recipes = std::vector<uint8_t>{3, 7};
    recipes.reserve(count + 10);
    auto elves = std::array<size_t, 2>{0, 1};

    std::string part1;
    std::optional<size_t> part2;

    auto push_value = [&](uint8_t value) {
      recipes.push_back(value);
      if (part1.size() < 10 && recipes.size() > count) {
        part1.push_back('0' + value);
      }
      if (!part2) {
        if (recipes.size() >= target.size() &&
            std::ranges::all_of(std::views::zip(recipes | std::views::reverse,
                                                target | std::views::reverse),
                                [](const auto &zipped) {
                                  return std::get<0>(zipped) ==
                                         std::get<1>(zipped);
                                })) {
          part2 = recipes.size() - target.size();
        }
      }
    };

    while (part1.size() != 10 || !part2) {
      uint8_t sum = recipes[elves[0]] + recipes[elves[1]];
      if (sum >= 10) {
        push_value(sum / 10);
      }
      push_value(sum % 10);
      elves[0] = (elves[0] + 1 + recipes[elves[0]]) % recipes.size();
      elves[1] = (elves[1] + 1 + recipes[elves[1]]) % recipes.size();
    }
    return std::make_pair(std::move(part1), *part2);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d14)

#else

#include <gtest/gtest.h>

TEST(d14, part1) {
  EXPECT_EQ(aoc::part1<d14>(aoc::arguments::make_example("9")), "5158916779");
  EXPECT_EQ(aoc::part1<d14>(aoc::arguments::make_example("5")), "0124515891");
  EXPECT_EQ(aoc::part1<d14>(aoc::arguments::make_example("18")), "9251071085");
  EXPECT_EQ(aoc::part1<d14>(aoc::arguments::make_example("2018")),
            "5941429882");
}

TEST(d14, part2) {
  EXPECT_EQ(aoc::part2<d14>(aoc::arguments::make_example("51589")), 9);
  EXPECT_EQ(aoc::part2<d14>(aoc::arguments::make_example("01245")), 5);
  EXPECT_EQ(aoc::part2<d14>(aoc::arguments::make_example("92510")), 18);
  EXPECT_EQ(aoc::part2<d14>(aoc::arguments::make_example("59414")), 2018);
}
#endif
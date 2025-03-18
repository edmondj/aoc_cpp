#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <algorithm>
#include <cassert>
#include <list>

struct data {
  size_t player_count;
  size_t marble_count;
};

struct d09 {
  static data convert(std::string_view input) {
    static const auto re =
        std::regex(R"((\d+) players; last marble is worth (\d+) points)");
    auto match = aoc::regex_match(aoc::trimmed(input), re).value();

    return data{.player_count = aoc::from_chars<size_t>(match.str(1)).value(),
                .marble_count = aoc::from_chars<size_t>(match.str(2)).value()};
  }

  static std::pair<size_t, size_t> run(const data &d) {
    auto scores = std::vector<size_t>(d.player_count, 0);
    auto marbles = std::list<size_t>{0};

    auto cur_marble = marbles.begin();
    size_t max_marble = 100 * d.marble_count;
    size_t part1;
    for (size_t i = 0; i < max_marble; ++i) {
      size_t marble = i + 1;
      if (marble % 23 != 0) {
        if (++cur_marble == marbles.end()) {
          cur_marble = marbles.begin();
        }
        cur_marble = marbles.insert(std::next(cur_marble), marble);
      } else {
        for (size_t j = 0; j < 7; ++j) {
          if (cur_marble == marbles.begin()) {
            cur_marble = marbles.end();
          }
          --cur_marble;
        }
        scores[i % scores.size()] += marble + *cur_marble;
        cur_marble = marbles.erase(cur_marble);
        if (cur_marble == marbles.end()) {
          cur_marble = marbles.begin();
        }
      }
      if (marble == d.marble_count) {
        part1 = std::ranges::max(scores);
      }
    }
    return {part1, std::ranges::max(scores)};
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d09)

#else

#include <gtest/gtest.h>

TEST(d09, part1) {
  EXPECT_EQ(aoc::part1<d09>(data{9, 25}), 32);
  EXPECT_EQ(aoc::part1<d09>(data{10, 1618}), 8317);
  EXPECT_EQ(aoc::part1<d09>(data{13, 7999}), 146373);
  EXPECT_EQ(aoc::part1<d09>(data{17, 1104}), 2764);
  EXPECT_EQ(aoc::part1<d09>(data{21, 6111}), 54718);
  EXPECT_EQ(aoc::part1<d09>(data{30, 5807}), 37305);
}

#endif
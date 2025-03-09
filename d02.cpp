#include <aoc_lib/string.hpp>

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct d02 {
  using input = std::vector<std::string_view>;

  static input convert(const std::string &input) {
    return std::vector{std::from_range,
                       std::views::split(aoc::trimmed(input), '\n') |
                           std::views::transform([](const auto &subrange) {
                             return aoc::trimmed(std::string_view(subrange));
                           })};
  }

  static uint64_t part1(const input &input) {
    uint64_t two_count = 0;
    uint64_t three_count = 0;
    for (std::string_view id : input) {
      std::unordered_map<char, uint64_t> letter_count;
      for (char c : id) {
        letter_count[c] += 1;
      }

      if (std::ranges::any_of(letter_count, [](const auto &entry) {
            return entry.second == 2;
          })) {
        two_count += 1;
      }
      if (std::ranges::any_of(letter_count, [](const auto &entry) {
            return entry.second == 3;
          })) {
        three_count += 1;
      }
    }
    return two_count * three_count;
  }

  static std::string part2(const input &input) {
    for (auto left_it = input.begin(); left_it != input.end(); ++left_it) {
      for (auto right_it = left_it + 1; right_it != input.end(); ++right_it) {
        if (left_it->size() != right_it->size()) {
          throw std::runtime_error("Box ids have mismatching sizes");
        }
        std::optional<size_t> single_mismatch;
        for (size_t i = 0; i < left_it->size(); ++i) {
          if (left_it->at(i) != right_it->at(i)) {
            if (single_mismatch) {
              single_mismatch = std::nullopt;
              break;
            } else {
              single_mismatch = i;
            }
          }
        }
        if (single_mismatch) {
          return std::string(*left_it).erase(*single_mismatch, 1);
        }
      }
    }
    throw std::runtime_error("Failed to find matching ids");
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d02)

#else

#include <gtest/gtest.h>

TEST(d02, part1) {
  EXPECT_EQ(d02::part1({"abcdef", "bababc", "abbcde", "abcccd", "aabcdd",
                        "abcdee", "ababab"}),
            12);
}

TEST(d02, part2) {
  EXPECT_EQ(d02::part2(d02::convert(R"(abcde
fghij
klmno
pqrst
fguij
axcye
wvxyz
    )")),
            "fgij");
}

#endif
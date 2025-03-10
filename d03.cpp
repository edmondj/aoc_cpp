#include <aoc_lib/string.hpp>

#include <cstdint>
#include <format>
#include <map>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

struct d03 {
  struct claim {
    uint64_t id;
    uint64_t x;
    uint64_t y;
    uint64_t width;
    uint64_t height;
  };

  static std::vector<claim> convert(const std::string &input) {
    return std::vector(
        std::from_range,
        aoc::lines(aoc::trimmed(input)) |
            std::views::transform([](std::string_view line) {
              static const auto re =
                  std::regex(R"(^#(\d+) @ (\d+),(\d+): (\d+)x(\d+)$)");
              std::match_results<std::string_view::const_iterator> match;

              if (!std::regex_match(line.begin(), line.end(), match, re)) {
                throw std::runtime_error(
                    std::format("Invalid format for claim: {}", line));
              }
              return claim{.id = *aoc::from_chars<uint64_t>(match[1].str()),
                           .x = *aoc::from_chars<uint64_t>(match[2].str()),
                           .y = *aoc::from_chars<uint64_t>(match[3].str()),
                           .width = *aoc::from_chars<uint64_t>(match[4].str()),
                           .height =
                               *aoc::from_chars<uint64_t>(match[5].str())};
            }));
  }

  static auto count_cell_claims(const std::vector<claim> &claims) {
    auto cell_claim_counts = std::map<std::pair<uint64_t, uint64_t>, size_t>();

    for (const claim &c : claims) {
      for (uint64_t i = 0; i < c.width; ++i) {
        for (uint64_t j = 0; j < c.height; ++j) {
          cell_claim_counts[{c.x + i, c.y + j}] += 1;
        }
      }
    }

    return cell_claim_counts;
  }

  static size_t part1(const std::vector<claim> &claims) {
    return std::ranges::count_if(
        count_cell_claims(claims),
        [](const auto &entry) { return entry.second > 1; });
  }

  static uint64_t part2(const std::vector<claim> &claims) {
    const auto counts = count_cell_claims(claims);

    for (const claim &c : claims) {
      for (uint64_t i = 0; i < c.width; ++i) {
        for (uint64_t j = 0; j < c.height; ++j) {
          if (counts.at({c.x + i, c.y + j}) > 1) {
            goto skip;
          }
        }
      }
      return c.id;
    skip:
      continue;
    }
    throw std::runtime_error("No non-overlapping claim found");
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d03)

#else

#include <gtest/gtest.h>

namespace {
const char *testData = R"(
#1 @ 1,3: 4x4
#2 @ 3,1: 4x4
#3 @ 5,5: 2x2
)";
}

TEST(d03, part1) { EXPECT_EQ(d03::part1(d03::convert(testData)), 4); }

TEST(d03, part2) { EXPECT_EQ(d03::part2(d03::convert(testData)), 3); }

#endif
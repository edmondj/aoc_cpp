#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/string.hpp>

#include <cassert>
#include <print>
#include <string>

struct d03 {

  static auto convert(const std::string &input) {
    return std::vector{
        std::from_range,
        aoc::lines(aoc::trimmed(input)) |
            std::views::transform([](std::string_view line) {
              return std::vector{
                  std::from_range,
                  aoc::trimmed(line) | std::views::transform([](char c) {
                    if (c < '0' || c > '9') {
                      throw std::runtime_error("Invalid digit");
                    }
                    return static_cast<uint8_t>(c - '0');
                  })};
            })};
  }

  template <size_t N>
  static auto possible_voltage(std::span<const uint8_t> range) {
    static_assert(N > 0);
    if constexpr (N == 1) {
      return range | std::views::transform(
                         [](auto i) { return static_cast<size_t>(i); });
    } else {
      return std::views::iota(0uz, range.size() - N + 1) |
             std::views::transform([range](auto i) {
               return possible_voltage<N - 1>(range.subspan(i + 1)) |
                      std::views::transform([i, range](auto value) {
                        return static_cast<size_t>(range[i]) *
                                   std::pow(10uz, N - 1) +
                               value;
                      });
             }) |
             std::views::join;
    }
  }

  template <size_t N> static auto max_voltage(std::span<const uint8_t> range) {
    static_assert(N > 0);
    assert(range.size() >= N);
    const auto search_space = range.subspan(0, range.size() - N + 1);
    const auto local_max = std::ranges::max_element(search_space);
    if constexpr (N == 1) {
      return static_cast<size_t>(*local_max);
    } else {
      return *local_max * std::pow(10, N - 1) +
             max_voltage<N - 1>(
                 range.subspan(local_max - search_space.begin() + 1));
    }
  }

  static auto part1(const auto &input) {
    return aoc::sum(input | std::views::transform([](const auto &row) {
                      return max_voltage<2>(std::span(row));
                    }))
        .value();
  }

  static auto part2(const auto &input) {
    return aoc::sum(input | std::views::transform([](const auto &row) {
                      return max_voltage<12>(std::span(row));
                    }))
        .value();
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d03)

#else

#include <gtest/gtest.h>

const auto TEST_DATA = aoc::arguments::make_example(R"(987654321111111
811111111111119
234234234234278
818181911112111
)");

TEST(d03, part1) { EXPECT_EQ(aoc::part1<d03>(TEST_DATA), 357); }
TEST(d03, part2) { EXPECT_EQ(aoc::part2<d03>(TEST_DATA), 3121910778619); }

#endif
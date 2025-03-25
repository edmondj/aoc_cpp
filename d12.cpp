#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <unordered_set>
#include <vector>

struct data {
  std::vector<bool> initial_state;
  std::unordered_set<uint8_t> growth_patterns;
};

template <typename T>
concept plants = std::ranges::input_range<T> &&
                 std::convertible_to<std::ranges::range_value_t<T>, bool>;

template <plants R> uint8_t make_plant_matcher(R &&range) {
  uint8_t res = 0;

  for (bool b : range) {
    res <<= 1;
    if (b) {
      res |= 1;
    }
  }

  return res;
}

constexpr size_t MAX_GENERATION_COUNT = 1000;

template <plants R> int64_t score_plants(R &&range) {
  return aoc::sum(range | std::views::enumerate |
                  std::views::filter([](const auto &enumerated) {
                    return std::get<1>(enumerated);
                  }) |
                  std::views::elements<0> |
                  std::views::transform([](size_t idx) {
                    return static_cast<int64_t>(idx) - MAX_GENERATION_COUNT * 2;
                  }));
}

struct d12 {
  static data convert(std::string_view input) {
    auto lines = aoc::lines(aoc::trimmed(input));
    auto cur = std::ranges::begin(lines);
    auto end = std::ranges::end(lines);

    if (cur == end) {
      throw std::runtime_error("Unexpected format: no header");
    }
    static const auto header_re = std::regex(R"(^initial state: ([\.#]+)$)");
    auto header_match = aoc::regex_match(*cur++, header_re)->str(1);
    data d{.initial_state = std::vector(header_match.size(), false)};
    std::ranges::transform(header_match, d.initial_state.begin(),
                           [](char c) { return c == '#'; });

    if (cur == end) {
      throw std::runtime_error("Unexpected format: missing separator");
    }
    if (*cur++ != "") {
      throw std::runtime_error("Unexpected format: non-empty separator");
    }
    for (std::string_view line : std::ranges::subrange(cur, end)) {
      static const auto rule_re = std::regex(R"(^([\\.#]{5}) => ([\\.#])$)");
      auto rule_match = *aoc::regex_match(line, rule_re);
      if (rule_match.str(2) == "#") {
        d.growth_patterns.insert(make_plant_matcher(
            rule_match.str(1) |
            std::views::transform([](char c) { return c == '#'; })));
      }
    }

    return d;
  }

  static std::pair<int64_t, int64_t> run(const data &d) {
    auto cur =
        std::vector(d.initial_state.size() + 4 * MAX_GENERATION_COUNT, false);
    std::ranges::copy(d.initial_state, cur.begin() + 2 * MAX_GENERATION_COUNT);
    auto cur_score = score_plants(cur);
    auto next = cur;
    size_t cur_generation = 0;
    int64_t part1;
    int64_t last_delta = 0;
    std::optional<int64_t> stable_delta;

    while (cur_generation < MAX_GENERATION_COUNT &&
           (cur_generation < 20 || !stable_delta)) {
      ++cur_generation;
      std::ranges::transform(
          cur | std::views::slide(5), next.begin() + 2, [&d](auto &&subrange) {
            return d.growth_patterns.contains(make_plant_matcher(subrange));
          });
      std::swap(cur, next);
      auto score = score_plants(cur);
      if (cur_generation == 20) {
        part1 = score;
      }
      auto delta = score - cur_score;
      if (delta == last_delta) {
        stable_delta = delta;
      }
      last_delta = delta;
      cur_score = score;
    }
    if (!stable_delta) {
      throw std::runtime_error("Could not find a stable delta");
    }

    return std::make_pair(part1, cur_score + (50000000000 - cur_generation) *
                                                 *stable_delta);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d12)

#else

#include <gtest/gtest.h>

namespace {
const auto test_data =
    aoc::arguments::make_example(R"(initial state: #..#.#..##......###...###

...## => #
..#.. => #
.#... => #
.#.#. => #
.#.## => #
.##.. => #
.#### => #
#.#.# => #
#.### => #
##.#. => #
##.## => #
###.. => #
###.# => #
####. => #
)");
}

TEST(d12, part1) { EXPECT_EQ(aoc::part1<d12>(test_data), 325); }

#endif
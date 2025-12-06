#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <print>
#include <string>

struct range_t {
  size_t from, to;

  bool includes(size_t v) const { return from <= v && v <= to; }
  size_t size() const { return to - from + 1; }
};

auto merge_ranges(const range_t &l, const range_t &r)
    -> std::optional<range_t> {
  if (l.from <= r.from && l.to >= r.to) {
    return l;
  }
  if (r.from <= l.from && r.to >= l.to) {
    return r;
  }
  if (l.from <= r.from && r.includes(l.to)) {
    return range_t{.from = l.from, .to = r.to};
  }
  if (r.includes(l.from) && l.to >= r.to) {
    return range_t{.from = r.from, .to = l.to};
  }
  return std::nullopt;
}

template <typename Char> struct std::formatter<range_t, Char> {
  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext &ctx) noexcept {
    return ctx.begin();
  }

  template <typename FmtContext>
  FmtContext::iterator format(const range_t &r, FmtContext &ctx) const {
    return std::format_to(ctx.out(), "{}-{}", r.from, r.to);
  }
};

struct input_t {
  std::vector<range_t> fresh_ranges;
  std::vector<size_t> available;
};

struct d05 {

  static auto convert(const std::string &input) -> input_t {
    auto blocks =
        aoc::lines(aoc::trimmed(input)) | std::views::split(std::string_view{});

    return input_t{
        .fresh_ranges =
            std::vector{
                std::from_range,
                *blocks.begin() |
                    std::views::transform([](std::string_view range) {
                      static const auto RE = std::regex(R"((\d+)-(\d+))");
                      auto match = aoc::regex_match(range, RE).value();
                      return range_t{
                          .from =
                              aoc::from_chars<size_t>(match[1].str()).value(),
                          .to =
                              aoc::from_chars<size_t>(match[2].str()).value()};
                    })},
        .available = std::vector{
            std::from_range, *std::ranges::next(blocks.begin()) |
                                 std::views::transform([](std::string_view id) {
                                   return aoc::from_chars<size_t>(id).value();
                                 })}};
  }

  static auto part1(const input_t &input) {
    return std::ranges::count_if(input.available, [&input](size_t id) {
      return std::ranges::any_of(
          input.fresh_ranges,
          [id](const range_t &range) { return range.includes(id); });
    });
  }

  static auto part2(const input_t &input) {
    auto final_ranges = std::vector<range_t>{};
    for (range_t r : input.fresh_ranges) {
      for (auto it = final_ranges.begin(); it != final_ranges.end();) {
        if (auto merge_result = merge_ranges(r, *it)) {
          r = *merge_result;
          it = final_ranges.erase(it);
        } else {
          ++it;
        }
      }
      final_ranges.push_back(r);
    }

    return aoc::sum(final_ranges | std::views::transform(&range_t::size))
        .value();
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d05)

#else

#include <gtest/gtest.h>

const auto TEST_DATA = aoc::arguments::make_example(R"(3-5
10-14
16-20
12-18

1
5
8
11
17
32)");

TEST(d05, part1) { EXPECT_EQ(aoc::part1<d05>(TEST_DATA), 3); }
TEST(d05, part2) { EXPECT_EQ(aoc::part2<d05>(TEST_DATA), 14); }

#endif
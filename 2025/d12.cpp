#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/fixed_matrix.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <cassert>
#include <print>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std::literals::string_view_literals;

using shape_t = aoc::fixed_matrix<bool, 3, 3>;

struct region_t {
  size_t width, height;
  std::vector<size_t> quantities;
};

struct input_t {
  std::vector<shape_t> shapes;
  std::vector<region_t> regions;
};

struct d12 {

  static auto convert(const std::string &input) -> input_t {
    auto shapes = std::vector<shape_t>{};
    auto regions = std::vector<region_t>{};
    auto lines = aoc::lines(aoc::trimmed(input));
    for (auto it = lines.begin(), end = lines.end(); it != end; ++it) {
      static const auto SHAPE_PATTERN = std::regex(R"(^(\d+):$)");
      static const auto REGION_PATTERN =
          std::regex(R"(^(\d+)x(\d+):((?: \d+)+)$)");

      if (auto shape_match = aoc::regex_match(*it, SHAPE_PATTERN)) {
        auto shape = shape_t{};
        for (size_t j = 0; j < 3; ++j) {
          std::string_view shape_line = *++it;
          assert(shape_line.size() == 3);
          for (size_t i = 0; i < 3; ++i) {
            shape.at(j, i) = shape_line[i] == '#';
          }
        }
        shapes.push_back(std::move(shape));
        ++it;
        assert((*it).empty());
        // parse shapes
      } else if (auto region_match = aoc::regex_match(*it, REGION_PATTERN)) {
        regions.push_back(region_t{
            .width = aoc::from_chars<size_t>(region_match->str(1)).value(),
            .height = aoc::from_chars<size_t>(region_match->str(2)).value(),
            .quantities = std::vector{
                std::from_range,
                aoc::split(aoc::trimmed(region_match->str(3)), ' ') |
                    std::views::transform([](std::string_view q) {
                      return aoc::from_chars<size_t>(q).value();
                    })}});

      } else {
        throw std::runtime_error(
            std::format("Unrecognized pattern: '{}'", *it));
      }
    }
    return {.shapes = std::move(shapes), .regions = std::move(regions)};
  }

  static auto part1(const input_t &input) {
    auto shape_sizes = std::vector{
        std::from_range,
        input.shapes | std::views::transform([](const shape_t &shape) {
          return std::ranges::count(shape.data(), true);
        })};
    return std::ranges::count_if(input.regions, [&input, &shape_sizes](
                                                    const region_t &region) {
      auto total_coverage =
          aoc::sum(region.quantities | std::views::enumerate |
                   std::views::transform(
                       [&shape_sizes](const std::tuple<size_t, size_t> &entry) {
                         return shape_sizes[std::get<0>(entry)] *
                                std::get<1>(entry);
                       }))
              .value();
      return total_coverage <= region.width * region.height;
    });
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d12)

#else

#include <gtest/gtest.h>

const auto TEST_DATA = aoc::arguments::make_example(R"(0:
###
##.
##.

1:
###
##.
.##

2:
.##
###
##.

3:
##.
###
##.

4:
###
#..
###

5:
###
.#.
###

4x4: 0 0 0 0 2 0
12x5: 1 0 1 0 2 2
12x5: 1 0 1 0 3 2
)");

TEST(d12, part1) { EXPECT_EQ(aoc::part1<d12>(TEST_DATA), 2); }

#endif
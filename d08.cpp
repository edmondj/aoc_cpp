#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/string.hpp>

#include <ranges>
#include <vector>

struct node {
  std::vector<node> children;
  std::vector<size_t> metadata;
};

size_t get_next(auto &it, auto end) {
  if (it == end) {
    throw std::runtime_error("Unexpected end of stream");
  }
  return *it++;
}

node parse_node(auto &it, auto end) {
  size_t child_size = get_next(it, end);
  size_t metadata_size = get_next(it, end);
  node res;

  for (size_t i = 0; i < child_size; ++i) {
    res.children.push_back(parse_node(it, end));
  }
  for (size_t i = 0; i < metadata_size; ++i) {
    res.metadata.push_back(get_next(it, end));
  }
  return res;
}

struct d08 {
  static node convert(std::string_view input) {
    auto range =
        input | std::views::split(' ') |
        std::views::transform([](auto subrange) {
          return aoc::from_chars<size_t>(std::string_view(subrange)).value();
        });
    auto begin = std::ranges::begin(range);
    return parse_node(begin, std::ranges::end(range));
  }

  static size_t part1(const node &n) {
    return aoc::sum(n.children | std::views::transform(&part1)) +
           aoc::sum(n.metadata);
  }

  static size_t part2(const node &n) {
    if (n.children.empty()) {
      return aoc::sum(n.metadata);
    }
    return aoc::sum(n.metadata |
                    std::views::transform([&n](size_t idx) -> size_t {
                      if (idx == 0 || idx > n.children.size()) {
                        return 0;
                      }
                      return part2(n.children[idx - 1]);
                    }));
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d08)

#else

#include <gtest/gtest.h>

namespace {
const auto test_data =
    aoc::arguments::make_example("2 3 0 3 10 11 12 1 1 0 1 99 2 1 1 2");
}

TEST(d08, part1) { EXPECT_EQ(aoc::part1<d08>(test_data), 138); }
TEST(d08, part2) { EXPECT_EQ(aoc::part2<d08>(test_data), 66); }

#endif
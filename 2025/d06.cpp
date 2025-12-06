#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/dyn_matrix.hpp>
#include <aoc_lib/string.hpp>

#include <cassert>
#include <string>

struct problem {
  size_t (*operand)(size_t, size_t) = nullptr;
  std::vector<size_t> values;
};

struct d06 {

  using input_t = std::pair<std::string, std::vector<problem>>;

  static auto convert(std::string input) -> input_t {
    auto res = std::vector<problem>{};
    auto lines = aoc::lines(aoc::trimmed(input));
    for (auto cur = lines.begin(), next = std::ranges::next(cur);
         cur != lines.end();) {
      for (auto [i, element] :
           aoc::split(*cur, ' ') | std::views::filter([](std::string_view e) {
             return !e.empty();
           }) | std::views::enumerate) {
        if (cur == lines.begin()) {
          res.emplace_back();
        }
        if (next == lines.end()) {
          if (element == "+") {
            res[i].operand = [](size_t l, size_t r) { return l + r; };
          } else if (element == "*") {
            res[i].operand = [](size_t l, size_t r) { return l * r; };
          } else {
            throw std::runtime_error(
                std::format("Unknown operand {}", element));
          }
        } else {
          res[i].values.push_back(aoc::from_chars<size_t>(element).value());
        }
      }
      cur = next;
      if (next != lines.end()) {
        ++next;
      }
    }
    return std::make_pair(std::move(input), std::move(res));
  }

  static auto part1(const input_t &input) {
    return aoc::sum(input.second | std::views::transform([](const problem &p) {
                      return std::ranges::fold_left(
                          std::ranges::subrange(p.values.begin() + 1,
                                                p.values.end()),
                          p.values.front(), p.operand);
                    }))
        .value();
  }

  static auto part2(const input_t &input) {
    auto grid = aoc::dyn_matrix{
        std::from_range,
        aoc::lines(input.first) | std::views::filter([](std::string_view line) {
          return !line.empty();
        })};
    size_t sum = 0;
    size_t local_problem = 0;
    size_t (*local_problem_op)(size_t, size_t) = nullptr;
    for (size_t x = 0; x <= grid.width(); ++x) {
      if (x == grid.width() ||
          std::ranges::all_of(
              std::views::iota(0uz, grid.height()),
              [&grid, x](size_t y) { return grid[{x, y}] == ' '; })) {
        // Commit local problem
        sum += local_problem;
        local_problem = 0;
        local_problem_op = nullptr;
      } else {
        size_t val = 0;
        for (size_t y = 0; y < grid.height() - 1; ++y) {
          auto c = grid[{x, y}];
          if (c == ' ') {
            continue;
          }
          if (c >= '0' && c <= '9') {
            val = val * 10 + c - '0';
          } else {
            throw std::runtime_error(
                std::format("Unexpected number digit '{}'", c));
          }
        }
        if (local_problem_op == nullptr) {
          auto c = grid[{x, grid.height() - 1}];
          if (c == '+') {
            local_problem_op = [](size_t l, size_t r) { return l + r; };
          } else if (c == '*') {
            local_problem_op = [](size_t l, size_t r) { return l * r; };
          } else {
            throw std::runtime_error(std::format("Unexpected operand {}", c));
          }
          local_problem = val;
        } else {
          local_problem = local_problem_op(local_problem, val);
        }
      }
    }
    return sum;
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d06)

#else

#include <gtest/gtest.h>

const auto TEST_DATA = aoc::arguments::make_example(R"(
123 328  51 64 
 45 64  387 23 
  6 98  215 314
*   +   *   +  
)");

TEST(d06, part1) { EXPECT_EQ(aoc::part1<d06>(TEST_DATA), 4277556); }
TEST(d06, part2) { EXPECT_EQ(aoc::part2<d06>(TEST_DATA), 3263827); }

#endif
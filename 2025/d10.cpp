#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/hash.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <z3++.h>

#include <cassert>
#include <iostream>
#include <print>
#include <string>
#include <unordered_map>

using value_t = uint16_t;
using joltage_t = std::array<value_t, sizeof(value_t) * 8>;

struct machine_t {
  value_t target{};
  std::vector<value_t> buttons;
  joltage_t joltage;
};

template <> struct std::hash<joltage_t> {
  size_t operator()(const joltage_t &j) const {
    auto acc = aoc::hash_accumulator{};
    for (value_t v : j) {
      acc.accumulate(v);
    }
    return acc.result();
  }
};

template <> struct std::hash<std::pair<joltage_t, size_t>> {
  size_t operator()(const std::pair<joltage_t, size_t> &p) const {
    auto acc = aoc::hash_accumulator{};
    acc.accumulate(std::hash<joltage_t>{}(p.first));
    acc.accumulate(p.second);
    return acc.result();
  }
};

using input_t = std::vector<machine_t>;

struct d10 {
  static auto convert(const std::string &input) {
    return std::vector{
        std::from_range,
        aoc::lines(aoc::trimmed(input)) |
            std::views::transform([](std::string_view line) {
              static const auto PATTERN =
                  std::regex(R"(^\[([#.]+)\] ((?:\([\d,]+\) )+)\{([\d,]+)\}$)");
              auto match = aoc::regex_match(line, PATTERN).value();

              assert(match[1].str().size() <= sizeof(value_t) * 8);
              auto target = value_t{};
              for (auto [i, c] : match[1].str() | std::views::enumerate) {
                if (c == '#') {
                  target |= (value_t{1} << i);
                }
              }

              auto buttons = std::vector{
                  std::from_range,
                  aoc::split(aoc::trimmed(match[2].str()), ' ') |
                      std::views::transform([](std::string_view button_str) {
                        auto button = value_t{};
                        for (std::string_view v : aoc::split(
                                 button_str.substr(1, button_str.size() - 2),
                                 ',')) {
                          button |= (value_t{1}
                                     << aoc::from_chars<value_t>(v).value());
                        }
                        return button;
                      })};

              auto joltage = joltage_t{};
              auto joltage_str = match[3].str();
              for (auto [i, v] :
                   aoc::split(joltage_str, ',') |
                       std::views::transform([](std::string_view joltage) {
                         return aoc::from_chars<value_t>(joltage).value();
                       }) |
                       std::views::enumerate) {
                joltage[i] = v;
              }
              return machine_t{.target = target,
                               .buttons = std::move(buttons),
                               .joltage = joltage};
            })};
  }

  static auto part1(const input_t &input) {
    return aoc::sum(
               input | std::views::transform([](const machine_t &machine) {
                 auto start = value_t{0};
                 auto dist = std::unordered_map<value_t, size_t>{{start, 0}};
                 auto to_visit =
                     std::vector<std::pair<size_t, value_t>>{{0, start}};

                 while (!to_visit.empty()) {
                   std::ranges::pop_heap(to_visit, std::greater{});
                   auto [cur_dist, cur] = to_visit.back();
                   to_visit.pop_back();
                   if (cur_dist > dist[cur]) {
                     continue;
                   }

                   auto next_dist = cur_dist + 1;
                   for (value_t button : machine.buttons) {
                     auto next = static_cast<value_t>(cur ^ button);
                     if (auto found = dist.find(next);
                         found == dist.end() || found->second > next_dist) {
                       dist.insert_or_assign(next, next_dist);
                       to_visit.push_back({next_dist, next});
                       std::ranges::push_heap(to_visit, std::greater{});
                     }
                   }
                 }
                 return dist.at(machine.target);
               }))
        .value();
  }

  static auto part2(const input_t &input) {
    return aoc::sum(
               input | std::views::transform([](const machine_t &machine) {
                 auto c = z3::context{};
                 auto o = z3::optimize(c);
                 auto dependencies = std::vector<z3::expr_vector>{};
                 for (size_t i = 0; i < machine.joltage.size(); ++i) {
                   dependencies.push_back(z3::expr_vector(c));
                 }
                 auto buttons = z3::expr_vector(c);
                 for (auto [bi, b] : machine.buttons | std::views::enumerate) {
                   auto button = c.int_const(std::format("b{}", bi).c_str());
                   o.add(button >= 0);
                   for (value_t i = 0; i < dependencies.size(); ++i) {
                     if ((b & (1 << i)) != 0) {
                       dependencies[i].push_back(button);
                     }
                   }
                   buttons.push_back(std::move(button));
                 }
                 for (size_t i = 0; i < dependencies.size(); ++i) {
                   auto s = c.int_const(std::format("s{}", i).c_str());
                   o.add(s == static_cast<int>(machine.joltage[i]));
                   if (!dependencies[i].empty()) {
                     o.add(s == z3::sum(dependencies[i]));
                   }
                 }
                 auto sum = c.int_const("sum");
                 o.add(sum == z3::sum(buttons));
                 o.minimize(sum);
                 if (o.check() != z3::sat) {
                   throw std::runtime_error("Failed to satisfy model");
                 }
                 return o.get_model().eval(sum).as_uint64();
               }))
        .value();
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d10)

#else

#include <gtest/gtest.h>

const auto TEST_DATA = aoc::arguments::make_example(
    R"([.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
[...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}
[.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}
)");

TEST(d10, part1) { EXPECT_EQ(aoc::part1<d10>(TEST_DATA), 7); }
TEST(d10, part2) { EXPECT_EQ(aoc::part2<d10>(TEST_DATA), 33); }

#endif
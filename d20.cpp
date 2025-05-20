#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/point.hpp>
#include <aoc_lib/geometry/vector.hpp>
#include <aoc_lib/string.hpp>

#include <print>
#include <stack>
#include <unordered_map>
#include <unordered_set>

using point_t = aoc::point2d<int64_t>;
using points_t = std::unordered_set<point_t>;
using vec_t = aoc::vector2d<int64_t>;
using graph_t = std::unordered_map<point_t, std::unordered_set<point_t>>;

void print_graph(const graph_t &g) {
  int64_t x_min, x_max, y_min, y_max;
  x_min = y_min = std::numeric_limits<int64_t>::max();
  x_max = y_max = std::numeric_limits<int64_t>::min();
  for (const auto &[p, n] : g) {
    x_min = std::min(x_min, p.x());
    x_max = std::max(x_max, p.x());
    y_min = std::min(y_min, p.y());
    y_max = std::max(y_max, p.y());
  }
  int64_t width = x_max - x_min + 1;
  int64_t height = y_max - y_min + 1;
  std::println("{}x{}", width, height);

  for (int64_t y = y_min; y <= y_max + 1; ++y) {
    for (int64_t x = x_min; x <= x_max + 1; ++x) {
      auto above = point_t{x, y - 1};
      bool exists = g.contains({x, y});
      bool above_exists = g.contains(above);
      std::print("{}", exists || above_exists || g.contains({x - 1, y}) ||
                               g.contains({x - 1, y - 1})
                           ? '#'
                           : ' ');
      std::print("{}",
                 !exists && !above_exists
                     ? ' '
                     : (exists && above_exists && g.at(above).contains({x, y})
                            ? '-'
                            : '#'));
    }
    std::println();
    if (y <= y_max) {
      for (int64_t x = x_min; x <= x_max + 1; ++x) {
        bool exists = g.contains({x, y});
        bool left_exists = g.contains({x - 1, y});
        std::print("{}", !exists && !left_exists
                             ? ' '
                             : (exists && left_exists &&
                                        g.at({x, y}).contains({x - 1, y})
                                    ? '|'
                                    : '#'));
        std::print("{}", x == 0 && y == 0 ? 'X' : (exists ? '.' : ' '));
      }
      std::println();
    }
  }
}

points_t move_all_points(const points_t &points, vec_t vec, graph_t &graph) {
  points_t result;
  result.reserve(points.size());
  for (point_t from : points) {
    point_t to = from + vec;
    result.insert(to);
    graph[from].insert(to);
    graph[to].insert(from);
  }
  return result;
}

graph_t build_graph(std::string_view input) {
  static const auto CARDINAL_DIRECTIONS =
      std::unordered_map<char, vec_t>{{'N', vec_t{0, -1}},
                                      {'E', vec_t{1, 0}},
                                      {'S', vec_t{0, 1}},
                                      {'W', vec_t{-1, 0}}};
  struct memory_t {
    points_t start;
    points_t accumulated;
  };

  std::stack<memory_t> memory;
  graph_t graph;
  auto points = points_t{{0, 0}};
  for (char c : input) {
    if (auto found = CARDINAL_DIRECTIONS.find(c);
        found != CARDINAL_DIRECTIONS.end()) {
      points = move_all_points(points, found->second, graph);
    } else if (c == '(') {
      memory.push({.start = points});
    } else if (c == '|') {
      if (memory.empty()) {
        throw std::runtime_error("Unexpected branch outside of parenthesis");
      }
      memory.top().accumulated.insert_range(points);
      points = memory.top().start;
    } else if (c == ')') {
      if (memory.empty()) {
        throw std::runtime_error(
            "Closing parenthesing without matching opening one");
      }
      points.insert_range(memory.top().accumulated);
      memory.pop();
    } else {
      throw std::runtime_error("Unexpected character in input");
    }
  }
  if (!memory.empty()) {
    throw std::runtime_error("Mismatch parenthesis");
  }
  return graph;
}

struct d20 {

  static graph_t convert(std::string_view input) {
    input = aoc::trimmed(input);
    if (input.starts_with('^')) {
      input = input.substr(1);
    }
    if (input.ends_with('$')) {
      input = input.substr(0, input.size() - 1);
    }
    return build_graph(input);
  }

  static std::pair<size_t, size_t> run(const graph_t &g) {
    auto to_visit = points_t{{0, 0}};
    auto visited = std::unordered_map<point_t, size_t>{{{0, 0}, 0}};
    size_t steps = 0;
    while (!to_visit.empty()) {
      ++steps;
      auto next_visit = points_t{};
      for (point_t p : to_visit) {
        for (point_t neighbor : g.at(p)) {
          if (visited.insert({neighbor, steps}).second) {
            next_visit.insert(neighbor);
          }
        }
      }
      to_visit = std::move(next_visit);
    }
    size_t part1 = 0;
    size_t part2 = 0;
    for (const auto [room, path_length] : visited) {
      part1 = std::max(part1, path_length);
      if (path_length >= 1000) {
        ++part2;
      }
    }
    return {part1, part2};
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d20)

#else

#include <gtest/gtest.h>

TEST(d20, part1) {
  EXPECT_EQ(aoc::part1<d20>(aoc::arguments::make_example("^WNE$")), 3);
  EXPECT_EQ(
      aoc::part1<d20>(aoc::arguments::make_example("^ENWWW(NEEE|SSE(EE|N))$")),
      10);
  EXPECT_EQ(aoc::part1<d20>(aoc::arguments::make_example(
                "^ENNWSWW(NEWS|)SSSEEN(WNSE|)EE(SWEN|)NNN$")),
            18);
  EXPECT_EQ(aoc::part1<d20>(aoc::arguments::make_example(
                "^ESSWWN(E|NNENN(EESS(WNSE|)SSS|WWWSSSSE(SW|NNNE)))$")),
            23);
  EXPECT_EQ(aoc::part1<d20>(aoc::arguments::make_example(
                "^WSSEESWWWNW(S|NENNEEEENN(ESSSSW(NWSW|SSEN)|WSWWN(E|WWS(E|"
                "SS))))$")),
            31);
}

#endif
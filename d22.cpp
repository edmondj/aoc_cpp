#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/point.hpp>
#include <aoc_lib/geometry/vector.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <generator>
#include <print>
#include <unordered_map>
#include <unordered_set>

using point2d = aoc::point2d<size_t>;

class cave_t {
public:
  cave_t(size_t depth, point2d target) : m_depth(depth), m_target(target) {}

  uint8_t region_type(point2d coordinate) {
    return erosion_level(coordinate) % 3;
  }

  point2d target() const { return m_target; }

private:
  size_t erosion_level(point2d coordinate) {
    auto found = m_erosion_levels.find(coordinate);
    if (found != m_erosion_levels.end()) {
      return found->second;
    }
    return m_erosion_levels
        .emplace(coordinate, (geologic_index(coordinate) + m_depth) % 20183)
        .first->second;
  }

  size_t geologic_index(point2d coordinate) {
    if (coordinate == point2d{0, 0} || coordinate == m_target)
      return 0;
    if (coordinate.y() == 0)
      return coordinate.x() * 16807;
    if (coordinate.x() == 0)
      return coordinate.y() * 48271;
    return erosion_level({coordinate.x() - 1, coordinate.y()}) *
           erosion_level({coordinate.x(), coordinate.y() - 1});
  }

  size_t m_depth;
  point2d m_target;
  std::unordered_map<point2d, size_t> m_erosion_levels;
};

enum class tool_t { torch, climbing_gear, neither };

bool is_region_compatible(uint8_t region, tool_t tool) {
  switch (region) {
  case 0:
    return tool == tool_t::climbing_gear || tool == tool_t::torch;
  case 1:
    return tool == tool_t::climbing_gear || tool == tool_t::neither;
  case 2:
    return tool == tool_t::torch || tool == tool_t::neither;
  default:
    std::unreachable();
  }
}

struct state {
  point2d coordinate;
  tool_t tool;

  bool operator==(const state &) const = default;
};

template <> struct std::hash<state> {
  size_t operator()(const state &s) const {
    return aoc::hash_combine(
        {std::hash<point2d>{}(s.coordinate), static_cast<size_t>(s.tool)});
  }
};

struct d22 {
  static cave_t convert(std::string_view input) {
    static const auto re =
        std::regex(R"(depth: (\d+)\s+target: (\d+),(\d+)\s*)");
    auto match = *aoc::regex_match(input, re);
    return cave_t{*aoc::from_chars<size_t>(match.str(1)),
                  {*aoc::from_chars<size_t>(match.str(2)),
                   *aoc::from_chars<size_t>(match.str(3))}};
  }

  static std::pair<size_t, size_t> run(cave_t cave) {
    size_t risk = 0;
    for (size_t y = 0; y <= cave.target().y(); ++y)
      for (size_t x = 0; x <= cave.target().x(); ++x) {
        risk += cave.region_type({x, y});
      }

    const auto start = state{{0, 0}, tool_t::torch};
    const auto target = state{cave.target(), tool_t::torch};
    auto visited = std::unordered_set<state>();
    auto to_visit = std::unordered_map<state, size_t>({{start, 0}});
    using timed_state = decltype(to_visit)::value_type;

    auto generate_neighbors =
        [&cave](const timed_state &ts) -> std::generator<timed_state> {
      auto [cur, time] = ts;
      // left
      if (cur.coordinate.x() > 0) {
        point2d next = {cur.coordinate.x() - 1, cur.coordinate.y()};
        if (is_region_compatible(cave.region_type(next), cur.tool)) {
          co_yield {{next, cur.tool}, time + 1};
        }
      }
      // up
      if (cur.coordinate.y() > 0) {
        point2d next = {cur.coordinate.x(), cur.coordinate.y() - 1};
        if (is_region_compatible(cave.region_type(next), cur.tool)) {
          co_yield {{next, cur.tool}, time + 1};
        }
      }
      // right
      {
        point2d next = {cur.coordinate.x() + 1, cur.coordinate.y()};
        if (is_region_compatible(cave.region_type(next), cur.tool)) {
          co_yield {{next, cur.tool}, time + 1};
        }
      }
      // down
      {
        point2d next = {cur.coordinate.x(), cur.coordinate.y() + 1};
        if (is_region_compatible(cave.region_type(next), cur.tool)) {
          co_yield {{next, cur.tool}, time + 1};
        }
      }
      // swapping tool
      for (tool_t next :
           {tool_t::torch, tool_t::climbing_gear, tool_t::neither}) {
        if (next != cur.tool &&
            is_region_compatible(cave.region_type(cur.coordinate), next)) {
          co_yield {{cur.coordinate, next}, time + 7};
        }
      }
    };

    while (!to_visit.empty()) {
      auto closest =
          std::ranges::min_element(to_visit, {}, &timed_state::second);
      auto cur = *closest;
      if (cur.first == target) {
        return {risk, cur.second};
      }
      to_visit.erase(closest);
      visited.insert(cur.first);
      for (auto [next, next_time] : generate_neighbors(cur)) {
        if (!visited.contains(next)) {
          auto [it, was_inserted] = to_visit.insert({next, next_time});
          if (!was_inserted) {
            it->second = std::min(it->second, next_time);
          }
        }
      }
    }
    throw std::runtime_error("Couldn't reach target");

    // return {risk, 0};
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d22)

#else

#include <gtest/gtest.h>

TEST(d22, part1) { EXPECT_EQ(aoc::part1<d22>({510, {10, 10}}), 114); }
TEST(d22, part2) { EXPECT_EQ(aoc::part2<d22>({510, {10, 10}}), 45); }

#endif
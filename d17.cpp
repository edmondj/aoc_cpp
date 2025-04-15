#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/dyn_matrix.hpp>
#include <aoc_lib/geometry/vector.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <cassert>
#include <print>
#include <unordered_map>

using point = aoc::point2d<int64_t>;
using vector = aoc::vector2d<int64_t>;

enum unit_t { sand, clay, still_water, running_water };

class units_t {
public:
  void set(point p, unit_t u) { m_map.insert_or_assign(p, u); }

  unit_t get(point p) const {
    auto found = m_map.find(p);
    if (found == m_map.end()) {
      return unit_t::sand;
    }
    return found->second;
  }

  auto begin() const { return m_map.begin(); }
  auto end() const { return m_map.end(); }

private:
  std::unordered_map<point, unit_t> m_map;
};

struct data_t {
  units_t map;
  int64_t upper_bound;
  int64_t lower_bound;
};

static const auto UP = vector{0, -1};
static const auto DOWN = vector{0, 1};
static const auto LEFT = vector{-1, 0};
static const auto RIGHT = vector{1, 0};

class water_simulation {
public:
  water_simulation(data_t data) : m_data(std::move(data)) {}

  void visit_source(point source) {
    point cur = source;
    while (cur.y() < m_data.lower_bound &&
           m_data.map.get(cur + DOWN) == unit_t::sand) {
      cur += DOWN;
    }
    while (cur != source) {
      if (cur.y() == m_data.lower_bound) {
        m_data.map.set(cur, unit_t::running_water);
      } else {
        switch (m_data.map.get(cur + DOWN)) {
        case unit_t::clay:
        case unit_t::still_water: {
          auto left_bound = horizontal_travel(cur, LEFT);
          auto right_bound = horizontal_travel(cur, RIGHT);
          bool is_still = true;
          if (m_data.map.get(left_bound) != unit_t::clay) {
            is_still = false;
            m_data.map.set(left_bound, unit_t::running_water);
          }
          if (m_data.map.get(right_bound) != unit_t::clay) {
            is_still = false;
            m_data.map.set(right_bound, unit_t::running_water);
          }
          for (int64_t x = left_bound.x() + 1; x < right_bound.x(); ++x) {
            m_data.map.set({x, cur.y()}, is_still ? unit_t::still_water
                                                  : unit_t::running_water);
          }
          break;
        }
        case unit_t::running_water:
          m_data.map.set(cur, unit_t::running_water);
          break;
        case unit_t::sand:
          std::unreachable();
        }
      }
      cur += UP;
    }
  }

  const units_t &map() const { return m_data.map; }

  template <std::ranges::input_range XR, std::ranges::input_range YR>
  void dump(XR &&xrange, YR &&yrange)
    requires(std::convertible_to<std::ranges::range_value_t<XR>, int64_t> &&
             std::convertible_to<std::ranges::range_value_t<YR>, int64_t>)
  {
    for (int64_t y : yrange) {
      for (int64_t x : xrange) {
        std::print("{}", [](unit_t u) {
          switch (u) {
          case unit_t::sand:
            return '.';
          case unit_t::clay:
            return '#';
          case unit_t::still_water:
            return '~';
          case unit_t::running_water:
            return '|';
          }
          std::unreachable();
        }(m_data.map.get(point{x, y})));
      }
      std::println();
    }
  }

private:
  point horizontal_travel(point cur, vector dir) {
    while (m_data.map.get(cur) == unit_t::sand &&
           m_data.map.get(cur + DOWN) != unit_t::running_water) {
      cur += dir;
      if (m_data.map.get(cur + DOWN) == unit_t::sand) {
        visit_source(cur);
      }
      if (m_data.map.get(cur + DOWN) == unit_t::running_water) {
        break;
      }
    }
    return cur;
  }

  // bool visit_source_impl(point source) {
  //   while (true) {
  //     point cur = source + DOWN;
  //     while (m_data.map.get(cur) == unit_t::sand) {
  //       cur += DOWN;
  //     }
  //     if (m_data.map.get(cur) == unit_t::running_water) {
  //       return true;
  //     }
  //   }
  //   point cur = source;
  //   while (true) {
  //     point down = cur + DOWN;
  //     if (unit_t u = m_data.map.get(down); u == unit_t::) {
  //       break;
  //     }
  //     if (down.y() > m_data.lower_bound) {
  //       return true;
  //     }
  //     cur = down;
  //     m_water_units.emplace(cur);
  //   }

  //   while (cur != source) {
  //     auto left_leaking = horizontal_travel(cur, LEFT);
  //     auto right_leaking = horizontal_travel(cur, RIGHT);
  //     if (left_leaking || right_leaking) {
  //       return true;
  //     }
  //     cur += UP;
  //   }
  //   return false;
  // }

  // // Returns false if the water is blocked in that direction
  // bool horizontal_travel(point cur, vector dir) {
  //   while (true) {
  //     auto next = cur + dir;
  //     if (m_data.clay.contains(next)) {
  //       return false;
  //     }
  //     cur = next;
  //     m_water_units.insert(cur);
  //     auto down = cur + DOWN;
  //     if (!m_data.clay.contains(down) && visit_source(cur)) {
  //       return true;
  //     }
  //   }
  // }

  data_t m_data;
  std::unordered_map<point, bool> m_sources;
  units_t m_water_units;
};

struct d17 {
  static data_t convert(std::string_view input) {
    static const auto re =
        std::regex(R"(^([x|y])=(\d+), ([x|y])=(\d+)\.\.(\d+)$)");

    struct vein_t {
      point start;
      vector dir;
      int64_t length;
    };

    units_t map;
    auto upper_bound = std::numeric_limits<int64_t>::max();
    int64_t lower_bound = 0;

    for (std::string_view line : aoc::lines(aoc::trimmed(input))) {
      auto match = *aoc::regex_match(line, re);
      assert(match.str(1) != match.str(3));
      auto lval = *aoc::from_chars<int64_t>(match.str(2));
      auto rlow = *aoc::from_chars<int64_t>(match.str(4));
      auto rhigh = *aoc::from_chars<int64_t>(match.str(5));
      assert(rlow <= rhigh);
      vein_t vein = [&]() {
        if (match.str(1) == "x") {
          return vein_t{.start = point{lval, rlow},
                        .dir = {0, 1},
                        .length = rhigh - rlow};
        } else {
          return vein_t{.start = point{rlow, lval},
                        .dir = {1, 0},
                        .length = rhigh - rlow};
        }
      }();
      for (int64_t i = 0; i <= vein.length; ++i) {
        auto p = vein.start + vein.dir * i;
        upper_bound = std::min(upper_bound, p.y());
        lower_bound = std::max(lower_bound, p.y());
        map.set(p, unit_t::clay);
      }
    }
    return {.map = std::move(map),
            .upper_bound = upper_bound,
            .lower_bound = lower_bound};
  }

  static std::pair<size_t, size_t> run(const data_t &d) {
    auto sim = water_simulation(d);
    sim.visit_source(point{500, d.upper_bound - 1});
    auto running = std::ranges::count(sim.map() | std::views::values,
                                      unit_t::running_water);
    auto still =
        std::ranges::count(sim.map() | std::views::values, unit_t::still_water);
    return std::make_pair(running + still, still);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d17)

#else

#include <gtest/gtest.h>

const auto test_data = aoc::arguments::make_example(R"(
x=495, y=2..7
y=7, x=495..501
x=501, y=3..7
x=498, y=2..4
x=506, y=1..2
x=498, y=10..13
x=504, y=10..13
y=13, x=498..504
)");

TEST(d17, part1) { EXPECT_EQ(aoc::part1<d17>(test_data), 57); }

#endif
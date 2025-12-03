#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/algorithm.hpp>
#include <aoc_lib/geometry/dyn_matrix.hpp>
#include <aoc_lib/geometry/point.hpp>
#include <aoc_lib/geometry/vector.hpp>
#include <aoc_lib/overload.hpp>
#include <aoc_lib/string.hpp>

#include <cassert>
#include <format>
#include <limits>
#include <set>
#include <unordered_map>
#include <variant>

using id_t = uint8_t;

constexpr struct wall_t {
} wall;
constexpr struct empty_t {
} empty;
struct goblin_t {
  id_t id;
};
struct elf_t {
  id_t id;
};
using cell_t = std::variant<wall_t, empty_t, goblin_t, elf_t>;

constexpr bool is_elf(const cell_t &cell) {
  return std::holds_alternative<elf_t>(cell);
}

constexpr bool is_goblin(const cell_t &cell) {
  return std::holds_alternative<goblin_t>(cell);
}

constexpr bool is_unit(const cell_t &cell) {
  return is_elf(cell) || is_goblin(cell);
}

constexpr bool is_empty(const cell_t &cell) {
  return std::holds_alternative<empty_t>(cell);
}

using cell_matcher = bool (*)(const cell_t &);

using hp_t = int16_t;

struct unit_state {
  hp_t hp;
};

constexpr bool is_alive(const unit_state &unit) { return unit.hp > 0; }
constexpr hp_t remaining_hp(const unit_state &unit) {
  return std::max(unit.hp, hp_t{});
}

using game_map = aoc::dyn_matrix<cell_t>;
using units = std::vector<unit_state>;

struct state_t {
  game_map map;
  units goblins;
  units elves;
};

using point2d = aoc::point2d<int64_t>;
using vector2d = aoc::vector2d<int64_t>;

constexpr vector2d read_directions[] = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}};

constexpr bool vector_contains(const vector2d &vector, const vector2d &sub) {
  if (sub.dx() != 0 && (vector.dx() == 0 ||
                        std::signbit(vector.dx()) != std::signbit(sub.dx()))) {
    return false;
  }
  if (sub.dy() != 0 && (vector.dy() == 0 ||
                        std::signbit(vector.dy()) != std::signbit(sub.dy()))) {
    return false;
  }
  return true;
}

struct reading_order_cmp {
  bool operator()(const point2d &l, const point2d &r) const {
    return l.y() < r.y() || (l.y() == r.y() && l.x() < r.x());
  }
};

using set = std::set<point2d, reading_order_cmp>;

// Returns the next position for the current unit
static point2d find_move(point2d start, const game_map &map,
                         cell_matcher target_matcher) {
  auto to_visit = set{start};
  auto previous = std::unordered_map<point2d, point2d>{};
  while (!to_visit.empty()) {
    auto next_visit = set{};
    for (point2d cur : to_visit) {
      for (vector2d dir : read_directions) {
        point2d next = cur + dir;
        if (next.x() >= 0 && next.y() >= 0 && map.contains(next)) {
          if (target_matcher(map[next])) {
            auto prev = cur;
            while (cur != start) {
              prev = cur;
              cur = previous.at(cur);
            }
            return prev;
          }
          if (is_empty(map[next]) && previous.emplace(next, cur).second) {
            next_visit.insert(next);
          }
        }
      }
    }
    to_visit = std::move(next_visit);
  }
  return start;
}

enum race_t { goblins, elves };

struct simulation_result {
  size_t rounds;
  race_t winners;
  units winner_states;

  size_t score() const {
    return rounds *
           *aoc::sum(winner_states | std::views::transform(remaining_hp));
  }
};

simulation_result run_simulation(state_t state, hp_t elf_damage) {
  size_t round = 0;
  while (true) {
    set turn_order;
    for (point2d p :
         aoc::views::point2d_iota(state.map.width(), state.map.height())) {
      if (is_unit(state.map[p])) {
        turn_order.insert(p);
      }
    }
    for (point2d cur : turn_order) {
      if (std::ranges::none_of(state.elves, is_alive)) {
        return {.rounds = round,
                .winners = race_t::goblins,
                .winner_states = std::move(state.goblins)};
      }
      if (std::ranges::none_of(state.goblins, is_alive)) {
        return {.rounds = round,
                .winners = race_t::elves,
                .winner_states = std::move(state.elves)};
      }
      auto enemy_matcher =
          std::visit(aoc::overload([](goblin_t) { return &is_elf; },
                                   [](elf_t) { return &is_goblin; },
                                   [](auto) -> cell_matcher {
                                     throw std::runtime_error(
                                         "Unexpected cell took a turn");
                                   }),
                     state.map[cur]);
      // Move
      {
        auto move = find_move(cur, state.map, enemy_matcher);
        if (move != cur) {
          std::swap(state.map[cur], state.map[move]);
          cur = move;
        }
      }
      // Attack
      {
        struct target_t {
          units &group;
          id_t id;
          hp_t damage;
        };
        std::optional<std::pair<point2d, target_t>> selected_target;
        for (auto dir : read_directions) {
          auto adjacent = cur + dir;
          if (adjacent.x() >= 0 && adjacent.y() >= 0 &&
              state.map.contains(adjacent) &&
              enemy_matcher(state.map[adjacent])) {
            target_t potential = std::visit(
                aoc::overload(
                    [&](const goblin_t &g) -> target_t {
                      return {state.goblins, g.id, elf_damage};
                    },
                    [&](const elf_t &e) -> target_t {
                      return {state.elves, e.id, 3};
                    },
                    [](auto) -> target_t {
                      throw std::runtime_error("Unexpected enemy cell");
                    }),
                state.map[adjacent]);
            const auto &[group, id, damage] = potential;
            if (!selected_target ||
                selected_target->second.group[selected_target->second.id].hp >
                    group[id].hp) {
              selected_target.emplace(std::make_pair(adjacent, potential));
            }
          }
        }
        if (selected_target) {
          auto [enemy, unit] = *selected_target;
          auto [group, id, damage] = unit;
          group[id].hp -= damage;
          if (!is_alive(group[id])) {
            state.map[enemy] = empty;
            turn_order.erase(enemy);
          }
        }
      }
    }
    ++round;
  }
}

struct d15 {
  static state_t convert(std::string_view input) {
    std::vector<cell_t> raw_map;
    size_t width = 0;
    size_t height = 0;
    raw_map.reserve(input.size());
    units goblins;
    units elves;

    for (std::string_view line : aoc::lines(aoc::trimmed(input))) {
      if (width == 0) {
        width = line.size();
      }
      ++height;
      assert(width == line.size());
      for (char c : line) {
        auto cell = [&]() -> cell_t {
          switch (c) {
          default:
            throw std::runtime_error(
                std::format("Unexpected map character: '{}'", c));
          case '.':
            return empty;
          case '#':
            return wall;
          case 'G':
            goblins.push_back({.hp = 200});
            return goblin_t{static_cast<id_t>(goblins.size() - 1)};
          case 'E':
            elves.push_back({.hp = 200});
            return elf_t{static_cast<id_t>(elves.size() - 1)};
          }
        }();
        raw_map.push_back(cell);
      }
    }
    raw_map.shrink_to_fit();
    return {.map = aoc::dyn_matrix<cell_t>(width, height, std::move(raw_map)),
            .goblins = std::move(goblins),
            .elves = std::move(elves)};
  }

  static size_t part1(state_t state) {
    return run_simulation(std::move(state), 3).score();
  }

  static size_t part2(const state_t &state) {
    if (auto lowest_result = run_simulation(state, 3);
        lowest_result.winners == race_t::elves &&
        std::ranges::all_of(lowest_result.winner_states, is_alive)) {
      return lowest_result.score();
    }
    simulation_result lowest_result = run_simulation(state, 200);
    if (lowest_result.winners != race_t::elves ||
        std::ranges::any_of(lowest_result.winner_states,
                            [](const unit_state &s) { return !is_alive(s); })) {
      throw std::runtime_error("Elves don't have a change of winning");
    }
    hp_t lower_bound = 3, upper_bound = 200;
    while (upper_bound - lower_bound > 1) {
      hp_t next = (upper_bound + lower_bound) / 2;
      auto result = run_simulation(state, next);
      if (result.winners == race_t::elves &&
          std::ranges::all_of(result.winner_states, is_alive)) {
        lowest_result = result;
        upper_bound = next;
      } else {
        lower_bound = next;
      }
    }
    return lowest_result.score();
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d15)

#else

#include <gtest/gtest.h>

struct test_data_t {
  aoc::arguments args;
  size_t part1;
  std::optional<size_t> part2;
};

const test_data_t test_data[]{
    {aoc::arguments::make_example(R"(
#######
#.G...#
#...EG#
#.#.#G#
#..G#E#
#.....#
#######
)"),
     27730, 4988},
    {aoc::arguments::make_example(R"(
#######
#G..#E#
#E#E.E#
#G.##.#
#...#E#
#...E.#
#######
)"),
     36334},
    {aoc::arguments::make_example(R"(
#######
#E..EG#
#.#G.E#
#E.##E#
#G..#.#
#..E#.#
#######
)"),
     39514, 31284},
    {aoc::arguments::make_example(R"(
#######
#E.G#.#
#.#G..#
#G.#.G#
#G..#.#
#...E.#
#######
)"),
     27755, 3478},
    {aoc::arguments::make_example(R"(
#######
#.E...#
#.#..G#
#.###.#
#E#G#G#
#...#G#
#######
)"),
     28944, 6474},
    {aoc::arguments::make_example(R"(
#########
#G......#
#.E.#...#
#..##..G#
#...##..#
#...#...#
#.G...G.#
#.....G.#
#########
)"),
     18740, 1140},
};

namespace tests {
struct d15 : testing::TestWithParam<test_data_t> {};

TEST_P(d15, example) {
  test_data_t test = GetParam();
  EXPECT_EQ(aoc::part1<::d15>(test.args), test.part1);
  if (test.part2) {
    EXPECT_EQ(aoc::part2<::d15>(test.args), test.part2);
  }
}

INSTANTIATE_TEST_SUITE_P(d15, d15, testing::ValuesIn(test_data),
                         [](const testing::TestParamInfo<test_data_t> &info) {
                           return testing::PrintToString(info.param.part1);
                         });

} // namespace tests
#endif
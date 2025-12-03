#include <aoc_lib/algorithm.hpp>
#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <ranges>
#include <unordered_set>
#include <vector>

using std::operator""sv;

// constexpr auto string_hash = [](std::string_view s) { return std::string(s);
// }; using damage_type_t = std::string;
constexpr auto string_hash = std::hash<std::string_view>{};
using damage_type_t = size_t;

struct effects_t {
  std::unordered_set<damage_type_t> weaknesses;
  std::unordered_set<damage_type_t> immunities;
};

effects_t parse_effects(std::string_view input) {
  static const auto effect_re =
      std::regex(R"(^(weak|immune) to (\w+(?:, (\w+))*)$)");

  effects_t sent;

  for (auto block : std::views::split(input, "; "sv)) {
    auto match = *aoc::regex_match(std::string_view(block), effect_re);
    auto &target = match.str(1) == "weak" ? sent.weaknesses : sent.immunities;
    for (auto type_range : std::views::split(match.str(2), ", "sv)) {
      auto type = std::string_view(type_range);
      if (!type.empty()) {
        target.insert(string_hash(type));
      }
    }
  }

  return sent;
}

struct group_t {
  std::string id;
  enum loyalty_t { immune_system, infection };
  loyalty_t loyalty;
  size_t unit_count;
  size_t hit_points;
  effects_t effects;
  size_t damage_per_unit;
  damage_type_t damage_type;
  size_t initiative;

  size_t effective_power() const { return unit_count * damage_per_unit; }

  size_t damage_to(const group_t &defender) const {
    if (defender.effects.immunities.contains(damage_type)) {
      return 0;
    }
    return effective_power() *
           (1 + defender.effects.weaknesses.contains(damage_type));
  }
};

group_t parse_group(std::string_view input, group_t::loyalty_t loyalty) {
  static const auto group_re = std::regex(
      R"(^(\d+) units each with (\d+) hit points (?:\((.*)\) )?with an attack that does (\d+) (\w+) damage at initiative (\d+)$)");
  auto match = *aoc::regex_match(input, group_re);
  return {.loyalty = loyalty,
          .unit_count = *aoc::from_chars<size_t>(match.str(1)),
          .hit_points = *aoc::from_chars<size_t>(match.str(2)),
          .effects = parse_effects(match.str(3)),
          .damage_per_unit = *aoc::from_chars<size_t>(match.str(4)),
          .damage_type = string_hash(match.str(5)),
          .initiative = *aoc::from_chars<size_t>(match.str(6))};
}

using data_t = std::vector<group_t>;

struct result_t {
  group_t::loyalty_t winner;
  size_t winner_units;
};

result_t run_fight(data_t d) {
  while (true) {
    size_t immune_system_units = 0;
    size_t infection_units = 0;
    for (const group_t &g : d) {
      auto &units = [&]() -> size_t & {
        switch (g.loyalty) {
        case group_t::immune_system:
          return immune_system_units;
        case group_t::infection:
          return infection_units;
        default:
          std::unreachable();
        }
      }();
      units += g.unit_count;
    }
    if (immune_system_units == 0) {
      return {group_t::infection, infection_units};
    }
    if (infection_units == 0) {
      return {group_t::immune_system, immune_system_units};
    }

    using attack_t = std::pair<size_t, std::optional<size_t>>;
    auto attack_plan = std::vector<attack_t>(
        std::from_range, std::views::iota(size_t(0), d.size()) |
                             std::views::transform([](size_t i) {
                               return std::make_pair(i, std::nullopt);
                             }));
    std::ranges::sort(attack_plan, {}, [&d](const attack_t &a) {
      const group_t &attacker = d[a.first];
      return std::make_tuple(
          attacker.loyalty,
          std::numeric_limits<size_t>::max() - attacker.effective_power(),
          std::numeric_limits<size_t>::max() - attacker.initiative);
    });

    for (auto &[attacker_idx, defender_idx] : attack_plan) {
      const group_t &attacker = d[attacker_idx];
      auto defending_candidates =
          std::views::iota(size_t(0), d.size()) |
          std::views::filter([&attack_plan](size_t idx) {
            return !std::ranges::contains(attack_plan, idx, &attack_t::second);
          }) |
          std::views::filter([&d, &attacker](size_t idx) {
            const group_t &defender = d[idx];
            return defender.loyalty != attacker.loyalty &&
                   !defender.effects.immunities.contains(attacker.damage_type);
          });

      if (!std::ranges::empty(defending_candidates)) {

        defender_idx = std::ranges::max(
            defending_candidates, {}, [&d, &attacker](size_t idx) {
              const group_t &defender = d[idx];
              return std::make_tuple(attacker.damage_to(defender),
                                     defender.effective_power(),
                                     defender.initiative);
            });
      }
    }

    std::ranges::sort(attack_plan, {}, [&d](const attack_t &attack) {
      return std::numeric_limits<size_t>::max() - d[attack.first].initiative;
    });

    size_t kill_count = 0;
    for (const auto &[attacker_idx, defender_idx] : attack_plan) {
      const auto &attacker = d[attacker_idx];
      if (defender_idx) {
        auto &defender = d[*defender_idx];
        size_t damage_done = attacker.damage_to(defender);
        size_t units_killed =
            std::min(damage_done / defender.hit_points, defender.unit_count);
        defender.unit_count -= units_killed;
        kill_count += units_killed;
      }
    }
    if (kill_count == 0) {
      // Stalemate, assume infection won
      std::erase_if(d, [](const group_t &g) {
        return g.loyalty == group_t::immune_system;
      });
    }
    std::erase_if(d, [](const group_t &g) { return g.unit_count == 0; });
  }
}

data_t boost(data_t d, size_t boost) {
  for (group_t &g : d) {
    if (g.loyalty == group_t::immune_system) {
      g.damage_per_unit += boost;
    }
  }
  return d;
}

struct d24 {
  static data_t convert(std::string_view input) {
    static const auto data_re = std::regex(
        R"(^Immune System:\r?\n([^]*)\r?\n\r?\nInfection:\r?\n?([^]*)$)");

    auto match = *aoc::regex_match(aoc::trimmed(input), data_re);
    auto sent = std::vector<group_t>();
    sent.append_range(aoc::lines(match.str(1)) |
                      std::views::transform([](std::string_view l) {
                        return parse_group(l, group_t::immune_system);
                      }));
    sent.append_range(aoc::lines(match.str(2)) |
                      std::views::transform([](std::string_view l) {
                        return parse_group(l, group_t::infection);
                      }));

    size_t immune_count = 0;
    size_t infection_count = 0;
    while (immune_count + infection_count < sent.size()) {
      group_t &g = sent[immune_count + infection_count];
      if (g.loyalty == group_t::immune_system) {
        g.id = std::format("Immune System group {}", ++immune_count);
      } else {
        g.id = std::format("Infection group {}", ++infection_count);
      }
    }
    return sent;
  }

  static std::pair<size_t, size_t> run(data_t d) {
    size_t lower_bound = 0;
    size_t part1 = run_fight(d).winner_units;
    size_t upper_bound = 128;
    size_t part2;
    while (true) {
      auto [winner, count] = run_fight(boost(d, upper_bound));
      if (winner == group_t::immune_system) {
        part2 = count;
        break;
      }
      lower_bound = upper_bound;
      upper_bound *= 2;
    }

    while ((upper_bound - lower_bound) > 1) {
      size_t mid_point = (upper_bound + lower_bound) / 2;
      auto [winner, count] = run_fight(boost(d, mid_point));
      if (winner == group_t::immune_system) {
        part2 = count;
        upper_bound = mid_point;
      } else {
        lower_bound = mid_point;
      }
    }
    return std::make_pair(part1, part2);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d24)

#else

#include <gtest/gtest.h>

const auto test_data = aoc::arguments::make_example(R"(
Immune System:
17 units each with 5390 hit points (weak to radiation, bludgeoning) with an attack that does 4507 fire damage at initiative 2
989 units each with 1274 hit points (immune to fire; weak to bludgeoning, slashing) with an attack that does 25 slashing damage at initiative 3

Infection:
801 units each with 4706 hit points (weak to radiation) with an attack that does 116 bludgeoning damage at initiative 1
4485 units each with 2961 hit points (immune to radiation; weak to fire, cold) with an attack that does 12 slashing damage at initiative 4
)");

TEST(d24, part1) { EXPECT_EQ(aoc::part1<d24>(test_data), 5216); }
TEST(d24, part2) { EXPECT_EQ(aoc::part2<d24>(test_data), 51); }

#endif
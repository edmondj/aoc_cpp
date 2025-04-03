#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <array>
#include <bitset>
#include <unordered_set>

using value_t = int64_t;
using registers_t = std::array<value_t, 4>;
using instruction_t = std::array<value_t, 4>;

enum class opcode_t : uint8_t {
  addr,
  addi,
  mulr,
  muli,
  banr,
  bani,
  borr,
  bori,
  setr,
  seti,
  gtir,
  gtri,
  gtrr,
  eqir,
  eqri,
  eqrr
};

struct sample_t {
  registers_t before;
  instruction_t instruction;
  registers_t after;
};

class opcode_candidates_t {
public:
  constexpr opcode_candidates_t() = default;
  constexpr opcode_candidates_t(std::initializer_list<opcode_t> opcodes) {
    for (opcode_t op : opcodes) {
      insert(op);
    }
  }

  constexpr void insert(opcode_t opcode) {
    flags.set(std::to_underlying(opcode));
  }

  constexpr bool erase(opcode_t opcode) {
    auto pos = std::to_underlying(opcode);
    if (flags.test(pos)) {
      flags.set(pos, false);
      return true;
    }
    return false;
  }

  constexpr auto size() const { return flags.count(); }

  constexpr bool operator==(const opcode_candidates_t &) const = default;

  constexpr opcode_t single_opcode() const {
    switch (flags.count()) {
    case 0:
      throw std::runtime_error("No opcode left");
    case 1: {
      for (std::underlying_type_t<opcode_t> i = 0; i < flags.size(); ++i) {
        if (flags.test(i)) {
          return static_cast<opcode_t>(i);
        }
      }
      std::unreachable();
    }
    default:
      throw std::runtime_error("Too many opcodes left");
    }
  }

  constexpr void assign_intersect(const opcode_candidates_t &r) {
    flags &= r.flags;
  }

private:
  std::bitset<16> flags;
};

value_t evaluate(opcode_t opcode, const instruction_t &inst,
                 const registers_t &reg) {
  switch (opcode) {
  case opcode_t::addr:
    return reg[inst[1]] + reg[inst[2]];
  case opcode_t::addi:
    return reg[inst[1]] + inst[2];
  case opcode_t::mulr:
    return reg[inst[1]] * reg[inst[2]];
  case opcode_t::muli:
    return reg[inst[1]] * inst[2];
  case opcode_t::banr:
    return reg[inst[1]] & reg[inst[2]];
  case opcode_t::bani:
    return reg[inst[1]] & inst[2];
  case opcode_t::borr:
    return reg[inst[1]] | reg[inst[2]];
  case opcode_t::bori:
    return reg[inst[1]] | inst[2];
  case opcode_t::setr:
    return reg[inst[1]];
  case opcode_t::seti:
    return inst[1];
  case opcode_t::gtir:
    return inst[1] > reg[inst[2]];
  case opcode_t::gtri:
    return reg[inst[1]] > inst[2];
  case opcode_t::gtrr:
    return reg[inst[1]] > reg[inst[2]];
  case opcode_t::eqir:
    return inst[1] == reg[inst[2]];
  case opcode_t::eqri:
    return reg[inst[1]] == inst[2];
  case opcode_t::eqrr:
    return reg[inst[1]] == reg[inst[2]];
  }
  std::unreachable();
}

opcode_candidates_t evaluate_candidates(const sample_t &sample) {
  opcode_candidates_t result;

  for (opcode_t op :
       {opcode_t::addr, opcode_t::addi, opcode_t::mulr, opcode_t::muli,
        opcode_t::banr, opcode_t::bani, opcode_t::borr, opcode_t::bori,
        opcode_t::setr, opcode_t::seti, opcode_t::gtir, opcode_t::gtri,
        opcode_t::gtrr, opcode_t::eqir, opcode_t::eqri, opcode_t::eqrr}) {
    value_t expected = sample.after[sample.instruction[3]];
    if (evaluate(op, sample.instruction, sample.before) == expected) {
      result.insert(op);
    }
  }

  return result;
}

using opcodes_t = std::array<opcode_t, 16>;

template <std::ranges::input_range R>
opcodes_t compute_opcodes(R &&range)
  requires(std::convertible_to<std::ranges::range_value_t<R>, const sample_t &>)
{
  std::array<opcode_candidates_t, 16> candidates;
  for (auto &candidate : candidates) {
    candidate = {
        opcode_t::addr, opcode_t::addi, opcode_t::mulr, opcode_t::muli,
        opcode_t::banr, opcode_t::bani, opcode_t::borr, opcode_t::bori,
        opcode_t::setr, opcode_t::seti, opcode_t::gtir, opcode_t::gtri,
        opcode_t::gtrr, opcode_t::eqir, opcode_t::eqri, opcode_t::eqrr};
  }

  for (const sample_t &s : range) {
    candidates[s.instruction[0]].assign_intersect(evaluate_candidates(s));
  }

  std::unordered_set<size_t> sure_things;
  for (size_t i = 0; i < candidates.size(); ++i) {
    if (candidates[i].size() == 1) {
      sure_things.insert(i);
    }
  }
  while (!sure_things.empty()) {
    size_t i = *sure_things.begin();
    sure_things.erase(sure_things.begin());
    for (size_t j = 0; j < candidates.size(); ++j) {
      auto &c = candidates[j];
      if (c.size() > 1 && c.erase(candidates[i].single_opcode()) &&
          c.size() == 1) {
        sure_things.insert(j);
      }
    }
  }

  std::array<opcode_t, 16> result;
  std::ranges::transform(candidates, std::ranges::begin(result),
                         [](const opcode_candidates_t &candidate) {
                           return candidate.single_opcode();
                         });
  return result;
}

struct data_t {
  std::vector<sample_t> samples;
  std::vector<instruction_t> instructions;
};

instruction_t parse_instructions_from_match(const aoc::svmatch &match) {
  return {*aoc::from_chars<value_t>(match.str(1)),
          *aoc::from_chars<value_t>(match.str(2)),
          *aoc::from_chars<value_t>(match.str(3)),
          *aoc::from_chars<value_t>(match.str(4))};
}

registers_t parse_registers_from_match(const aoc::svmatch &match) {
  return parse_instructions_from_match(match);
}

struct d16 {
  static data_t convert(std::string_view input) {
    static const auto before_re =
        std::regex(R"(^Before: \[(\d), (\d), (\d), (\d)\]$)");
    static const auto instruction_re = std::regex(R"((\d\d?) (\d) (\d) (\d))");
    static const auto after_re =
        std::regex(R"(^After:  \[(\d), (\d), (\d), (\d)\]$)");

    data_t result;
    bool parsing_samples = true;
    for (auto subrange : aoc::lines(aoc::trimmed(input)) |
                             std::views::split(std::string_view{})) {
      if (std::ranges::empty(subrange)) {
        parsing_samples = false;
      } else {
        if (parsing_samples) {
          auto cur = std::ranges::begin(subrange);
          result.samples.push_back(sample_t{
              .before = parse_registers_from_match(
                  *aoc::regex_match(*cur++, before_re)),
              .instruction = parse_instructions_from_match(
                  *aoc::regex_match(*cur++, instruction_re)),
              .after = parse_registers_from_match(
                  *aoc::regex_match(*cur++, after_re)),
          });
        } else {
          for (std::string_view line : subrange) {
            result.instructions.push_back(parse_instructions_from_match(
                *aoc::regex_match(line, instruction_re)));
          }
        }
      }
    }
    return result;
  }

  static auto part1(const data_t &data) {
    return std::ranges::count_if(data.samples, [](const sample_t &s) {
      return evaluate_candidates(s).size() >= 3;
    });
  }

  static value_t part2(const data_t &data) {
    auto opcodes = compute_opcodes(data.samples);

    auto reg = registers_t{};

    for (const instruction_t &inst : data.instructions) {
      reg[inst[3]] = evaluate(opcodes[inst[0]], inst, reg);
    }

    return reg[0];
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d16)

#else

#include <gtest/gtest.h>

TEST(d16, candidates) {
  auto got = evaluate_candidates({{3, 2, 1, 1}, {9, 2, 1, 2}, {3, 2, 2, 1}});
  auto expected =
      opcode_candidates_t{opcode_t::mulr, opcode_t::addi, opcode_t::seti};
  EXPECT_EQ(got, expected);
}

#endif
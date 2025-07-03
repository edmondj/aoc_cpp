#include "device.hpp"

#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/overload.hpp>

#include <fstream>
#include <iterator>
#include <print>
#include <ranges>
#include <unordered_set>
#include <variant>

using device::program_t;
using device::value_t;

#define SIMPLE_FORMATTER(TYPE, VALUE_NAME, OUT_NAME)                           \
  template <typename Out> Out simple_format(const TYPE &, Out);                \
                                                                               \
  template <typename Char> struct std::formatter<TYPE, Char> {                 \
    template <typename ParseCtx> constexpr auto parse(ParseCtx &ctx) {         \
      return ctx.begin();                                                      \
    }                                                                          \
    template <typename FormatCtx>                                              \
    constexpr auto format(const TYPE &value, FormatCtx &ctx) const {           \
      return simple_format(value, ctx.out());                                  \
    }                                                                          \
  };                                                                           \
  template <typename Out>                                                      \
  Out simple_format(const TYPE &VALUE_NAME, Out OUT_NAME)

namespace ast {
struct register_t {
  register_t() = default;
  register_t(value_t idx, std::optional<value_t> ip)
      : is_ip(idx == ip), idx(idx) {}

  bool is_ip : 1 = false;
  value_t idx : 63 = -1;

  constexpr bool operator==(const register_t &) const = default;
};
SIMPLE_FORMATTER(ast::register_t, r, out) {
  if (r.is_ip)
    return std::format_to(out, "ip");
  return std::format_to(out, "r{}", r.idx);
}

struct addr_t {
  register_t a;
  register_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::addr_t, i, out) {
  return std::format_to(out, "{} = {} + {}", i.c, i.a, i.b);
}

struct addi_t {
  register_t a;
  value_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::addi_t, i, out) {
  return std::format_to(out, "{} = {} + {}", i.c, i.a, i.b);
}

struct mulr_t {
  register_t a;
  register_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::mulr_t, i, out) {
  return std::format_to(out, "{} = {} * {}", i.c, i.a, i.b);
}

struct muli_t {
  register_t a;
  value_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::muli_t, i, out) {
  return std::format_to(out, "{} = {} * {}", i.c, i.a, i.b);
}

struct banr_t {
  register_t a;
  register_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::banr_t, i, out) {
  return std::format_to(out, "{} = {} & {}", i.c, i.a, i.b);
}

struct bani_t {
  register_t a;
  value_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::bani_t, i, out) {
  return std::format_to(out, "{} = {} & {}", i.c, i.a, i.b);
}

struct borr_t {
  register_t a;
  register_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::borr_t, i, out) {
  return std::format_to(out, "{} = {} | {}", i.c, i.a, i.b);
}

struct bori_t {
  register_t a;
  value_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::bori_t, i, out) {
  return std::format_to(out, "{} = {} | {}", i.c, i.a, i.b);
}

struct setr_t {
  register_t a;
  register_t c;
};
SIMPLE_FORMATTER(ast::setr_t, i, out) {
  return std::format_to(out, "{} = {}", i.c, i.a);
}

struct seti_t {
  value_t a;
  register_t c;
};
SIMPLE_FORMATTER(ast::seti_t, i, out) {
  return std::format_to(out, "{} = {}", i.c, i.a);
}

struct gtir_t {
  value_t a;
  register_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::gtir_t, i, out) {
  return std::format_to(out, "{} = {} > {}", i.c, i.a, i.b);
}

struct gtri_t {
  register_t a;
  value_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::gtri_t, i, out) {
  return std::format_to(out, "{} = {} > {}", i.c, i.a, i.b);
}

struct gtrr_t {
  register_t a;
  register_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::gtrr_t, i, out) {
  return std::format_to(out, "{} = {} > {}", i.c, i.a, i.b);
}

struct eqir_t {
  value_t a;
  register_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::eqir_t, i, out) {
  return std::format_to(out, "{} = {} == {}", i.c, i.a, i.b);
}

struct eqri_t {
  register_t a;
  value_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::eqri_t, i, out) {
  return std::format_to(out, "{} = {} == {}", i.c, i.a, i.b);
}

struct eqrr_t {
  register_t a;
  register_t b;
  register_t c;
};
SIMPLE_FORMATTER(ast::eqrr_t, i, out) {
  return std::format_to(out, "{} = {} == {}", i.c, i.a, i.b);
}

struct jmp_t {
  value_t label;
};
SIMPLE_FORMATTER(ast::jmp_t, i, out) {
  return std::format_to(out, "jmp {:02}:", i.label);
}

struct exit_t {};
SIMPLE_FORMATTER(ast::exit_t, i, out) {
  (void)i;
  return std::format_to(out, "exit");
}

// jump if
struct jif_t {
  register_t test;
  value_t label;
};
SIMPLE_FORMATTER(ast::jif_t, i, out) {
  return std::format_to(out, "jif {} {:02}:", i.test, i.label);
}

// exit if
struct exif_t {
  register_t test;
};
SIMPLE_FORMATTER(ast::exif_t, i, out) {
  return std::format_to(out, "exif {}", i.test);
}

struct instruction_t {
  using variant_t =
      std::variant<addr_t, addi_t, mulr_t, muli_t, banr_t, bani_t, borr_t,
                   bori_t, setr_t, seti_t, gtir_t, gtri_t, gtrr_t, eqir_t,
                   eqri_t, eqrr_t, jmp_t, exit_t, jif_t, exif_t>;

  instruction_t() = default;
  template <std::convertible_to<variant_t> T>
  instruction_t(T &&v) : instruction(std::forward<T>(v)) {}

  std::optional<value_t> label;

  variant_t instruction;

  std::optional<register_t> output() const {
    return std::visit(
        [](auto &&v) -> std::optional<register_t> {
          if constexpr (requires { v.c; }) {
            return v.c;
          } else {
            return std::nullopt;
          }
        },
        instruction);
  }

  bool is_cmp() const {
    return std::visit(
        []<typename T>(const T &) {
          return std::same_as<T, eqir_t> || std::same_as<T, eqri_t> ||
                 std::same_as<T, eqrr_t> || std::same_as<T, gtir_t> ||
                 std::same_as<T, gtri_t> || std::same_as<T, gtrr_t>;
        },
        instruction);
  }
};
SIMPLE_FORMATTER(ast::instruction_t, i, out) {
  if (i.label) {
    out = std::format_to(out, "{:02}:\n", *i.label);
  }
  return std::visit(
      [&out](const auto &i) { return std::format_to(out, "\t{}", i); },
      i.instruction);
}

instruction_t convert_instruction(const device::instruction_t &from,
                                  std::optional<value_t> ip) {
  switch (from.opcode) {
  case device::opcode_t::addr:
    return addr_t{{from.args[0], ip}, {from.args[1], ip}, {from.args[2], ip}};
  case device::opcode_t::addi:
    return addi_t{{from.args[0], ip}, from.args[1], {from.args[2], ip}};
  case device::opcode_t::mulr:
    return mulr_t{{from.args[0], ip}, {from.args[1], ip}, {from.args[2], ip}};
  case device::opcode_t::muli:
    return muli_t{{from.args[0], ip}, from.args[1], {from.args[2], ip}};
  case device::opcode_t::banr:
    return banr_t{{from.args[0], ip}, {from.args[1], ip}, {from.args[2], ip}};
  case device::opcode_t::bani:
    return bani_t{{from.args[0], ip}, from.args[1], {from.args[2], ip}};
  case device::opcode_t::borr:
    return borr_t{{from.args[0], ip}, {from.args[1], ip}, {from.args[2], ip}};
  case device::opcode_t::bori:
    return bori_t{{from.args[0], ip}, from.args[1], {from.args[2], ip}};
  case device::opcode_t::setr:
    return setr_t{{from.args[0], ip}, {from.args[2], ip}};
  case device::opcode_t::seti:
    return seti_t{from.args[0], {from.args[2], ip}};
  case device::opcode_t::gtir:
    return gtir_t{from.args[0], {from.args[1], ip}, {from.args[2], ip}};
  case device::opcode_t::gtri:
    return gtri_t{{from.args[0], ip}, from.args[1], {from.args[2], ip}};
  case device::opcode_t::gtrr:
    return gtrr_t{{from.args[0], ip}, {from.args[1], ip}, {from.args[2], ip}};
  case device::opcode_t::eqir:
    return eqir_t{from.args[0], {from.args[1], ip}, {from.args[2], ip}};
  case device::opcode_t::eqri:
    return eqri_t{{from.args[0], ip}, from.args[1], {from.args[2], ip}};
  case device::opcode_t::eqrr:
    return eqrr_t{{from.args[0], ip}, {from.args[1], ip}, {from.args[2], ip}};
  default:
    std::unreachable();
  }
}

struct block_t {
  std::vector<instruction_t> instructions;
};

} // namespace ast

void dump_help(const program_t &p) {
  ast::block_t main;
  main.instructions.reserve(p.instructions.size());
  for (const device::instruction_t &i : p.instructions) {
    main.instructions.push_back(ast::convert_instruction(i, p.ip));
  }

  // Rewrite jumps
  for (auto [ip, instruction] : main.instructions | std::views::enumerate) {
    if (auto out = instruction.output(); out && out->is_ip) {
      std::visit(
          [&](auto i) {
            if constexpr (std::same_as<std::remove_cvref_t<decltype(i)>,
                                       ast::seti_t>) {
              value_t target = i.a + 1;
              if (target < 0 ||
                  static_cast<size_t>(target) >= main.instructions.size()) {
                instruction.instruction = ast::exit_t{};
              } else {
                main.instructions[target].label = target;
                instruction.instruction = ast::jmp_t{target};
              }
              return;
            } else if constexpr (std::same_as<std::remove_cvref_t<decltype(i)>,
                                              ast::addi_t>) {
              if (i.a.is_ip) {
                value_t target = ip + i.b + 1;
                if (target < 0 ||
                    static_cast<size_t>(target) >= main.instructions.size()) {
                  instruction.instruction = ast::exit_t{};
                } else {
                  main.instructions[target].label = target;
                  instruction.instruction = ast::jmp_t{target};
                }
                return;
              }
              std::println("TODO: {}", instruction);
            } else if constexpr (std::same_as<std::remove_cvref_t<decltype(i)>,
                                              ast::addr_t>) {
              if (i.a.is_ip != i.b.is_ip && ip > 0) {
                ast::register_t test = !i.a.is_ip ? i.a : i.b;
                if (main.instructions[ip - 1].output() == test &&
                    main.instructions[ip - 1].is_cmp()) {
                  value_t target = ip + 2;
                  if (target < 0 ||
                      static_cast<size_t>(target) >= main.instructions.size()) {
                    instruction.instruction = ast::exif_t{test};
                  } else {
                    main.instructions[target].label = target;
                    instruction.instruction = ast::jif_t{test, target};
                  }
                  return;
                }
              }
              std::println("TODO: {}", instruction);
            } else {
              std::println("TODO: {}", instruction);
            }
          },
          instruction.instruction);
    }
  }

  auto out_file = std::ofstream("d21_help.txt");
  auto out = std::ostream_iterator<char>(out_file);
  for (const auto &i : main.instructions) {
    std::format_to(out, "{}\n", i);
  }
}

struct d21 {
  static program_t convert(std::string_view input) {
    return device::parse_program(input);
  }

  static std::array<value_t, 2> run(const program_t &) {
    // Uncomment this to get insight on your program if you don't share the same
    // condition
    // dump_help(p);

    auto solutions = std::unordered_set<value_t>();
    auto first = std::optional<value_t>();
    auto last = std::optional<value_t>();
    value_t r3 = 0, r4 = 0;
    // The program can be simplified to the following, most likely you'd have to
    // change the magic numbers
    while (true) {
      r3 = r4 | 65536;
      r4 = 4332021;
      while (true) {
        r4 = (((r4 + (r3 & 255)) & 16777215) * 65899) & 16777215;
        if (256 > r3)
          break;
        r3 = (r3 / 256);
      }
      if (!solutions.insert(r4).second)
        break;
      if (!first)
        first = r4;
      last = r4;
    }
    return {*first, *last};
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d21)

#else

#endif
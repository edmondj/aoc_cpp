#pragma once

#include <array>
#include <format>
#include <string_view>
#include <utility>

namespace device {

using value_t = int64_t;
template <size_t N> using registers_t = std::array<value_t, N>;

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

constexpr std::pair<opcode_t, std::string_view> OPCODE_LABELS[] = {
    {opcode_t::addr, "addr"}, {opcode_t::addi, "addi"},
    {opcode_t::mulr, "mulr"}, {opcode_t::muli, "muli"},
    {opcode_t::banr, "banr"}, {opcode_t::bani, "bani"},
    {opcode_t::borr, "borr"}, {opcode_t::bori, "bori"},
    {opcode_t::setr, "setr"}, {opcode_t::seti, "seti"},
    {opcode_t::gtir, "gtir"}, {opcode_t::gtri, "gtri"},
    {opcode_t::gtrr, "gtrr"}, {opcode_t::eqir, "eqir"},
    {opcode_t::eqri, "eqri"}, {opcode_t::eqrr, "eqrr"},
};

struct instruction_t {
  opcode_t opcode;
  std::array<value_t, 3> args;
};

template <size_t N>
void evaluate(const instruction_t &inst, registers_t<N> &reg) {
  reg[inst.args[2]] = [&]() -> value_t {
    switch (inst.opcode) {
    case opcode_t::addr:
      return reg[inst.args[0]] + reg[inst.args[1]];
    case opcode_t::addi:
      return reg[inst.args[0]] + inst.args[1];
    case opcode_t::mulr:
      return reg[inst.args[0]] * reg[inst.args[1]];
    case opcode_t::muli:
      return reg[inst.args[0]] * inst.args[1];
    case opcode_t::banr:
      return reg[inst.args[0]] & reg[inst.args[1]];
    case opcode_t::bani:
      return reg[inst.args[0]] & inst.args[1];
    case opcode_t::borr:
      return reg[inst.args[0]] | reg[inst.args[1]];
    case opcode_t::bori:
      return reg[inst.args[0]] | inst.args[1];
    case opcode_t::setr:
      return reg[inst.args[0]];
    case opcode_t::seti:
      return inst.args[0];
    case opcode_t::gtir:
      return inst.args[0] > reg[inst.args[1]];
    case opcode_t::gtri:
      return reg[inst.args[0]] > inst.args[1];
    case opcode_t::gtrr:
      return reg[inst.args[0]] > reg[inst.args[1]];
    case opcode_t::eqir:
      return inst.args[0] == reg[inst.args[1]];
    case opcode_t::eqri:
      return reg[inst.args[0]] == inst.args[1];
    case opcode_t::eqrr:
      return reg[inst.args[0]] == reg[inst.args[1]];
    }
    std::unreachable();
  }();
}

instruction_t parse_instruction(std::string_view s);

std::string_view label_for(opcode_t op);
} // namespace device

template <typename Char> struct std::formatter<device::opcode_t, Char> {

  template <class ParseContext>
  constexpr ParseContext::iterator parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <class FmtContext>
  FmtContext::iterator format(device::opcode_t opcode, FmtContext &ctx) const {
    return std::format_to(ctx.out(), "{}", device::label_for(opcode));
  }
};
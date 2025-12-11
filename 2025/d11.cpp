#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <print>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std::literals::string_view_literals;

using id_t = uint16_t;

constexpr auto YOU = id_t{0};
constexpr auto OUT = id_t{1};
constexpr auto SVR = id_t{2};
constexpr auto DAC = id_t{3};
constexpr auto FFT = id_t{4};

using input_t = std::vector<std::unordered_set<uint16_t>>;

size_t paths_to_out(id_t from, const input_t &graph,
                    std::unordered_map<id_t, size_t> &cache) {
  if (from == OUT) {
    return 1;
  }
  if (auto found = cache.find(from); found != cache.end()) {
    return found->second;
  }
  auto res = 0uz;
  for (id_t next : graph[from]) {
    res += paths_to_out(next, graph, cache);
  }
  cache.insert_or_assign(from, res);
  return res;
}

class traversal_state_t {
public:
  traversal_state_t(id_t id) : m_state(id) {}

  id_t id() const { return m_state & 0x0000FFFF; }
  bool is_final() const { return (m_state >> (sizeof(id_t) * 8)) == 3; }
  void set_dac() { m_state |= (1 << sizeof(id_t) * 8); }
  void set_fft() { m_state |= (1 << ((sizeof(id_t) * 8) + 1)); }

  traversal_state_t next(id_t id) const {
    return {from_state, (m_state & 0xFFFF0000) | id};
  }

  constexpr auto operator<=>(const traversal_state_t &) const = default;

private:
  struct from_state_t {};
  static constexpr from_state_t from_state;

  traversal_state_t(from_state_t, uint32_t state) : m_state(state) {}

  uint32_t m_state{};

  friend struct std::hash<traversal_state_t>;
};

template <> struct std::hash<traversal_state_t> {
  size_t operator()(traversal_state_t t) const { return t.m_state; }
};

size_t
paths_to_dac_fft_out(traversal_state_t from, const input_t &graph,
                     std::unordered_map<traversal_state_t, size_t> &cache) {
  if (from.id() == OUT) {
    return from.is_final();
  }
  if (from.id() == DAC) {
    from.set_dac();
  } else if (from.id() == FFT) {
    from.set_fft();
  }
  if (auto found = cache.find(from); found != cache.end()) {
    return found->second;
  }
  auto res = 0uz;
  for (id_t next : graph[from.id()]) {
    res += paths_to_dac_fft_out(from.next(next), graph, cache);
  }
  cache.insert_or_assign(from, res);
  return res;
}

struct d11 {

  static auto convert(const std::string &input) -> input_t {
    auto id_labels =
        std::vector<std::string_view>{"you", "out", "svr", "dac", "fft"};
    auto res = input_t{5};

    auto get_id = [&id_labels](std::string_view label) {
      auto it = std::ranges::find(id_labels, label);
      if (it == id_labels.end()) {
        id_labels.push_back(label);
        return static_cast<id_t>(id_labels.size() - 1);
      }
      return static_cast<id_t>(it - id_labels.begin());
    };

    for (std::string_view line : aoc::lines(aoc::trimmed(input))) {
      auto splitted = aoc::split(line, ": "sv);
      auto it = std::ranges::begin(splitted);
      auto cur = get_id(*it++);
      if (res.size() <= cur) {
        res.resize(cur + 1);
      }
      res[cur] = {std::from_range, aoc::split(aoc::trimmed(*it), ' ') |
                                       std::views::transform(get_id)};
    }

    return res;
  }

  static auto part1(const input_t &input) {
    auto cache = std::unordered_map<id_t, size_t>{};
    return paths_to_out(YOU, input, cache);
  }

  static auto part2(const input_t &input) {
    auto cache = std::unordered_map<traversal_state_t, size_t>{};
    return paths_to_dac_fft_out(SVR, input, cache);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d11)

#else

#include <gtest/gtest.h>

const auto TEST_DATA = aoc::arguments::make_example(R"(aaa: you hhh
you: bbb ccc
bbb: ddd eee
ccc: ddd eee fff
ddd: ggg
eee: out
fff: out
ggg: out
hhh: ccc fff iii
iii: out
)");

TEST(d11, part1) { EXPECT_EQ(aoc::part1<d11>(TEST_DATA), 5); }

const auto TEST_DATA_2 = aoc::arguments::make_example(R"(svr: aaa bbb
aaa: fft
fft: ccc
bbb: tty
tty: ccc
ccc: ddd eee
ddd: hub
hub: fff
eee: dac
dac: fff
fff: ggg hhh
ggg: out
hhh: out
)");

TEST(d11, part2) { EXPECT_EQ(aoc::part2<d11>(TEST_DATA_2), 2); }
#endif
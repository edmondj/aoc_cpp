#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry/point.hpp>
#include <aoc_lib/string.hpp>

#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

#include <aoc_lib/geometry_format.hpp>
#include <print>

using point_t = aoc::point3d<int64_t>;
using input_t = std::pair<size_t, std::vector<point_t>>;

struct d08 {

  static auto convert(const aoc::arguments &input) -> input_t {
    return std::make_pair(
        input.is_example ? 10uz : 1000uz,
        std::vector{
            std::from_range,
            aoc::lines(aoc::trimmed(input.input)) |
                std::views::transform([](std::string_view line) {
                  return point_t{
                      std::from_range,
                      aoc::split(line, ',') |
                          std::views::transform([](std::string_view elem) {
                            return aoc::from_chars<int64_t>(elem).value();
                          })};
                })});
  }

  static auto run(const input_t &in) {
    struct heap_elem_t {
      int64_t dist;
      point_t from, to;

      constexpr auto operator<=>(const heap_elem_t &) const = default;
    };

    auto [to_take, input] = in;

    auto circuits = std::vector<std::unordered_set<point_t>>{
        std::from_range, input | std::views::transform([](const point_t &p) {
                           return std::unordered_set{p};
                         })};
    auto distance_heap = std::vector<heap_elem_t>{};
    distance_heap.reserve(input.size() * input.size());
    for (auto it = input.begin(); it != input.end(); ++it) {
      for (auto next = it + 1; next != input.end(); ++next) {
        distance_heap.push_back(heap_elem_t{
            aoc::squared_euclidean_distance(*it, *next), *it, *next});
        std::ranges::push_heap(distance_heap, std::greater{});
      }
    }
    size_t part1 = 0, part2 = 0;
    for (size_t i = 1; !distance_heap.empty(); ++i) {
      std::ranges::pop_heap(distance_heap, std::greater{});
      auto &c = distance_heap.back();
      auto new_circuit = std::unordered_set{c.from, c.to};
      new_circuit.reserve(input.size());
      for (auto it = circuits.begin(); it != circuits.end();) {
        if (std::ranges::any_of(new_circuit, [&it](const point_t &p) {
              return it->contains(p);
            })) {
          new_circuit.merge(*it);
          it = circuits.erase(it);
        } else {
          ++it;
        }
      }
      circuits.push_back(std::move(new_circuit));

      // Part 1
      if (i == to_take) {
        auto sizes =
            std::vector{std::from_range,
                        circuits | std::views::transform(
                                       &std::unordered_set<point_t>::size)};
        std::ranges::sort(sizes, std::greater{});
        part1 = std::ranges::fold_left(sizes | std::views::take(3), 1uz,
                                       std::multiplies{});
      }

      // Part 2
      if (circuits.size() == 1) {
        part2 = c.from.x() * c.to.x();
        break;
      }

      distance_heap.pop_back();
    }
    return std::make_pair(part1, part2);
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d08)

#else

#include <gtest/gtest.h>

const auto TEST_DATA = aoc::arguments::make_example(R"(162,817,812
57,618,57
906,360,560
592,479,940
352,342,300
466,668,158
542,29,236
431,825,988
739,650,466
52,470,668
216,146,977
819,987,18
117,168,530
805,96,715
346,949,466
970,615,88
941,993,340
862,61,35
984,92,344
425,690,689
)");

TEST(d08, part1) { EXPECT_EQ(aoc::part1<d08>(TEST_DATA), 40); }
TEST(d08, part2) { EXPECT_EQ(aoc::part2<d08>(TEST_DATA), 25272); }

#endif
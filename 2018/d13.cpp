#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/geometry.hpp>
#include <aoc_lib/string.hpp>

#include <format>
#include <map>
#include <stdexcept>

using point = aoc::point2d<int64_t>;
using vector = aoc::vector2d<int64_t>;
using matrix = aoc::matrix2d<int64_t>;
using mine_map = aoc::dyn_matrix<char>;

constexpr matrix turn_left = {0, 1, -1, 0};
constexpr matrix turn_right = {0, -1, 1, 0};
constexpr matrix turn_matrices[] = {turn_left, matrix::identity(), turn_right};

struct cart_data {
  vector velocity = {0, 0};
  size_t turn_memory = 0;
};

struct cart_selector {
  bool operator()(const point &l, const point &r) const {
    return l.y() < r.y() || (l.y() == r.y() && l.x() < r.x());
  }
};

using cart_map = std::map<point, cart_data, cart_selector>;

struct data {
  mine_map mine;
  cart_map carts;
};

struct d13 {
  static data convert(std::string_view input) {
    std::vector<char> mine;
    mine.reserve(input.size());
    std::size_t width = 0;
    std::size_t height = 0;
    auto lines = aoc::lines(input) |
                 std::views::drop_while(
                     [](std::string_view line) { return line.empty(); }) |
                 std::views::take_while(
                     [](std::string_view line) { return !line.empty(); });
    cart_map carts;
    for (auto &&[y, line] : lines | std::views::enumerate) {
      std::size_t line_width = 0;
      for (auto &&[x, c] : line | std::views::enumerate) {
        char to_insert = c;
        switch (to_insert) {
        case ' ':
        case '|':
        case '-':
        case '+':
        case '/':
        case '\\':
          // valid chars, nothing to do
          break;
        // carts
        case '>':
          to_insert = '-';
          carts.insert_or_assign({x, y}, cart_data{.velocity = {1, 0}});
          break;
        case 'v':
          to_insert = '|';
          carts.insert_or_assign({x, y}, cart_data{.velocity = {0, 1}});
          break;
        case '<':
          to_insert = '-';
          carts.insert_or_assign({x, y}, cart_data{.velocity = {-1, 0}});
          break;
        case '^':
          to_insert = '|';
          carts.insert_or_assign({x, y}, cart_data{.velocity = {0, -1}});
          break;
        default:
          throw std::runtime_error(
              std::format("Invalid char '{}' in input", to_insert));
        }
        mine.push_back(to_insert);
        ++line_width;
      }
      if (y == 0) {
        width = line_width;
      } else if (width != line_width) {
        throw std::runtime_error("Inconsistent line size in input");
      }
      ++height;
    }

    return {.mine = mine_map(width, height, std::move(mine)),
            .carts = std::move(carts)};
  }

  static std::array<std::string, 2> run(const data &d) {
    cart_map carts = d.carts;
    std::optional<point> first_crash;
    while (carts.size() > 1) {
      cart_map new_carts;
      while (!carts.empty()) {
        auto pos = carts.begin()->first;
        auto data = carts.begin()->second;
        carts.erase(carts.begin());

        pos += data.velocity;
        switch (d.mine[pos.y(), pos.x()]) {
        case '-':
        case '|':
          break;
        case '+':
          data.velocity = turn_matrices[data.turn_memory] * data.velocity;
          data.turn_memory = (data.turn_memory + 1) % std::size(turn_matrices);
          break;
        case '/':
          if (data.velocity.dx() != 0) {
            data.velocity = turn_left * data.velocity;
          } else if (data.velocity.dy() != 0) {
            data.velocity = turn_right * data.velocity;
          }
          break;
        case '\\':
          if (data.velocity.dx() != 0) {
            data.velocity = turn_right * data.velocity;
          } else if (data.velocity.dy() != 0) {
            data.velocity = turn_left * data.velocity;
          }
          break;
        default:
          std::unreachable();
        }
        if (auto found = carts.find(pos); found != carts.end()) {
          if (!first_crash) {
            first_crash = pos;
          }
          carts.erase(found);
        } else {
          auto [where, inserted] = new_carts.emplace(pos, data);
          if (!inserted) {
            if (!first_crash) {
              first_crash = pos;
            }
            new_carts.erase(where);
          }
        }
      }
      carts = std::move(new_carts);
    }
    point last_cart = carts.empty() ? point{} : carts.begin()->first;
    return {std::format("{},{}", first_crash->x(), first_crash->y()),
            std::format("{},{}", last_cart.x(), last_cart.y())};
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d13)

#else

#include <gtest/gtest.h>

const auto part1_test_data = aoc::arguments::make_example(R"(
/->-\        
|   |  /----\
| /-+--+-\  |
| | |  | v  |
\-+-/  \-+--/
  \------/   
)");

TEST(d13, part1) { EXPECT_EQ(aoc::part1<d13>(part1_test_data), "7,3"); }

const auto part2_test_data = aoc::arguments::make_example(R"(
/>-<\  
|   |  
| /<+-\
| | | v
\>+</ |
  |   ^
  \<->/
)");

TEST(d13, part2) { EXPECT_EQ(aoc::part2<d13>(part2_test_data), "6,4"); }

#endif
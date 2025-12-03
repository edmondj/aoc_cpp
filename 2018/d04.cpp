#include <aoc_lib/overload.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <chrono>
#include <format>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

struct timestamp {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;

  constexpr auto operator<=>(const timestamp &) const = default;

  // Returns difference in minute
  int64_t operator-(const timestamp &rhs) const {
    if (year != rhs.year || month != rhs.month) {
      throw std::runtime_error(
          "Can not compare timestamps from different months");
    }
    return int64_t((day * 24 + hour) * 60 + minute) -
           int64_t((rhs.day * 24 + rhs.hour) * 60 + rhs.minute);
  }
};

struct shift_start {
  uint64_t id;
};
struct wakes_up {};
struct falls_asleep {};
using log_entry = std::variant<shift_start, wakes_up, falls_asleep>;

using records = std::map<timestamp, log_entry>;

template <typename CharT> struct std::formatter<timestamp, CharT> {

  template <typename ParseCtx>
  constexpr ParseCtx::iterator parse(ParseCtx &ctx) {
    return ctx.begin();
  }

  template <typename FmtCtx>
  FmtCtx::iterator format(const timestamp &time, FmtCtx &ctx) const {
    return std::format_to(ctx.out(), "{:04}-{:02}-{:02} {:02}:{:02}", time.year,
                          time.month, time.day, time.hour, time.minute);
  }
};

struct guard_sleep_stats {
  uint64_t duration = 0;
  std::unordered_map<uint8_t, uint64_t> per_minute;
};

using sleep_stats = std::unordered_map<uint64_t, guard_sleep_stats>;

struct d04 {
  static records parse_records(std::string_view input) {
    return records{
        std::from_range,
        aoc::lines(aoc::trimmed(input)) | std::views::transform([](std::string_view
                                                                       line) {
          static const auto re = std::regex(
              R"(^\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2})\] (?:(falls asleep)|(wakes up)|Guard #(\d+) begins shift)$)");

          auto match_result = aoc::regex_match(line, re);
          if (!match_result) {
            throw std::runtime_error(
                std::format("Failed to match entry {}", line));
          }
          auto match = std::move(*match_result);

          return std::make_pair(
              timestamp{
                  .year = *aoc::from_chars<uint16_t>(match[1].str()),
                  .month = *aoc::from_chars<uint8_t>(match[2].str()),
                  .day = *aoc::from_chars<uint8_t>(match[3].str()),
                  .hour = *aoc::from_chars<uint8_t>(match[4].str()),
                  .minute = *aoc::from_chars<uint8_t>(match[5].str()),
              },
              [&]() -> log_entry {
                if (match[6].length() > 0) {
                  return falls_asleep();
                }
                if (match[7].length() > 0) {
                  return wakes_up();
                }
                if (match[8].length() > 0) {
                  return shift_start(
                      *aoc::from_chars<uint64_t>(match[8].str()));
                }
                std::unreachable();
              }());
        })};
  }

  static sleep_stats convert(const std::string &input) {
    auto records = parse_records(input);
    sleep_stats stats_per_guard;

    std::optional<uint64_t> current_guard;
    std::optional<timestamp> last_sleep_time;
    for (const auto &[time, entry] : records) {
      std::visit(
          aoc::overload(
              [&](const shift_start &shift) {
                if (last_sleep_time) {
                  throw std::runtime_error(std::format(
                      "New shift started while guard was sleeping at time {}",
                      time));
                }
                current_guard = shift.id;
              },
              [&](const wakes_up &) {
                if (!current_guard) {
                  throw std::runtime_error(std::format(
                      "Waking up but guard is not set at time {}", time));
                }
                if (last_sleep_time) {
                  auto &stats = stats_per_guard[*current_guard];
                  uint64_t duration = time - *last_sleep_time;
                  stats.duration += duration;
                  for (uint64_t i = 0; i < duration; ++i) {
                    stats.per_minute[(last_sleep_time->minute + i) % 60] += 1;
                  }
                  last_sleep_time = std::nullopt;
                }
              },
              [&](const falls_asleep &) {
                if (!current_guard) {
                  throw std::runtime_error(std::format(
                      "Waking up but guard is not set at time {}", time));
                }
                if (!last_sleep_time) {
                  last_sleep_time = time;
                }
              }),
          entry);
    }
    return stats_per_guard;
  }

  static uint64_t part1(const sleep_stats &input) {
    auto found_stat =
        std::ranges::max_element(input, [](const auto &l, const auto &r) {
          return l.second.duration < r.second.duration;
        });
    if (found_stat == input.end()) {
      throw std::runtime_error("No guard candidate found");
    }
    auto found_minute = std::ranges::max_element(
        found_stat->second.per_minute,
        [](const auto &l, const auto &r) { return l.second < r.second; });
    if (found_minute == found_stat->second.per_minute.end()) {
      throw std::runtime_error("No prefered minute found");
    }

    return found_stat->first * found_minute->first;
  }

  static uint64_t part2(const sleep_stats &input) {
    auto [guard, minute, duration] = std::ranges::max(
        input | std::views::transform([](const auto &e) {
          auto [minute, duration] = *std::ranges::max_element(
              e.second.per_minute, {}, [](const auto &e) { return e.second; });
          return std::make_tuple(e.first, minute, duration);
        }),
        {}, [](const auto &e) { return std::get<2>(e); });

    return guard * minute;
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d04)

#else

#include <gtest/gtest.h>

namespace {
const char *test_data = R"(
[1518-11-01 00:00] Guard #10 begins shift
[1518-11-01 00:05] falls asleep
[1518-11-01 00:25] wakes up
[1518-11-01 00:30] falls asleep
[1518-11-01 00:55] wakes up
[1518-11-01 23:58] Guard #99 begins shift
[1518-11-02 00:40] falls asleep
[1518-11-02 00:50] wakes up
[1518-11-03 00:05] Guard #10 begins shift
[1518-11-03 00:24] falls asleep
[1518-11-03 00:29] wakes up
[1518-11-04 00:02] Guard #99 begins shift
[1518-11-04 00:36] falls asleep
[1518-11-04 00:46] wakes up
[1518-11-05 00:03] Guard #99 begins shift
[1518-11-05 00:45] falls asleep
[1518-11-05 00:55] wakes up
)";
}

TEST(d04, part1) { EXPECT_EQ(d04::part1(d04::convert(test_data)), 240); }
TEST(d04, part2) { EXPECT_EQ(d04::part2(d04::convert(test_data)), 4455); }

#endif
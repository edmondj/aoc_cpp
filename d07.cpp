#include <aoc_lib/day_trait.hpp>
#include <aoc_lib/regex.hpp>
#include <aoc_lib/string.hpp>

#include <algorithm>
#include <format>
#include <map>
#include <set>
#include <stdexcept>

struct d07 {
  struct data {
    std::map<char, std::set<char>> requirements;
    size_t num_workers;
    size_t min_task_time;
  };

  static data convert(const aoc::arguments &args) {
    auto sent = data{
        .num_workers = args.is_example ? 2u : 5u,
        .min_task_time = args.is_example ? 0u : 60u,
    };
    for (std::string_view line : aoc::lines(aoc::trimmed(args.input))) {
      static const auto re = std::regex(
          R"(Step ([A-Z]) must be finished before step ([A-Z]) can begin.)");
      if (auto match = aoc::regex_match(line, re)) {
        auto before = match->str(1).front();
        auto step = match->str(2).front();
        sent.requirements[before]; // Creates the entry if none exists
        sent.requirements[step].insert(before);
      } else {
        throw std::runtime_error(std::format("Can not match line '{}'", line));
      }
    }
    return sent;
  }

  static std::map<size_t, std::set<char>> create_timeline(data d) {
    std::map<size_t, std::set<char>> timeline;
    size_t now = 0;
    std::map<size_t, std::set<char>> pending_tasks;

    while (!d.requirements.empty() || !pending_tasks.empty()) {
      while (pending_tasks.size() < d.num_workers) {
        auto it = std::ranges::find_if(d.requirements, [](const auto &entry) {
          return entry.second.empty();
        });
        if (it == d.requirements.end()) {
          break;
        }
        pending_tasks[now + d.min_task_time + it->first - 'A' + 1].insert(
            it->first);
        d.requirements.erase(it);
      }
      // Pop the next task
      if (pending_tasks.empty()) {
        if (!d.requirements.empty()) {
          throw std::runtime_error(
              std::format("Entered a locked situation at time {}", now));
        }
        continue;
      }
      auto &[time, tasks] = *pending_tasks.begin();
      for (auto &[_, req] : d.requirements) {
        for (char t : tasks) {
          req.erase(t);
        }
      }
      timeline[time].insert_range(tasks);
      now = time;
      pending_tasks.erase(pending_tasks.begin());
    }
    return timeline;
  }

  static std::string part1(data d) {
    auto res = std::string{};
    res.reserve(d.requirements.size());

    // Force 1 worker and minimum time
    d.num_workers = 1;
    d.min_task_time = 0;

    for (const auto &[_, tasks] : create_timeline(std::move(d))) {
      res.append_range(tasks);
    }
    return res;
  }

  static size_t part2(data d) {
    auto timeline = create_timeline(std::move(d));
    return timeline.rbegin()->first;
  }
};

#ifndef TESTING

#include <aoc_main/main.hpp>

AOC_MAIN(d07)

#else

#include <gtest/gtest.h>

namespace {
const auto testData = aoc::arguments::make_example(R"(
Step C must be finished before step A can begin.
Step C must be finished before step F can begin.
Step A must be finished before step B can begin.
Step A must be finished before step D can begin.
Step B must be finished before step E can begin.
Step D must be finished before step E can begin.
Step F must be finished before step E can begin.
)");
}

TEST(d07, part1) { EXPECT_EQ(aoc::part1<d07>(testData), "CABDFE"); }
TEST(d07, part2) { EXPECT_EQ(aoc::part2<d07>(testData), 15); }

#endif
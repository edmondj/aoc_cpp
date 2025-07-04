#pragma once

#include <utility>

namespace aoc {

class hash_accumulator {
public:
  void accumulate(size_t hash);

  size_t result() const;

private:
  size_t m_hash = 0;
};

size_t hash_combine(std::initializer_list<size_t> hashes);

}
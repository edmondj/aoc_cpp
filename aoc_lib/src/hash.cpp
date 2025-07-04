#include "aoc_lib/hash.hpp"

namespace aoc {

void hash_accumulator::accumulate(size_t hash) {
  m_hash = hash + 0x9e3779b9 + (m_hash << 6) + (m_hash >> 2);
}

size_t hash_accumulator::result() const { return m_hash; }

size_t hash_combine(std::initializer_list<size_t> hashes) {
  hash_accumulator acc;
  for (size_t h : hashes)
    acc.accumulate(h);
  return acc.result();
}

} // namespace aoc
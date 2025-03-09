add_library(aoc_lib)

find_package(CLI11 REQUIRED)

target_sources(
  aoc_lib
  PUBLIC public/aoc_lib/input.hpp public/aoc_lib/day_trait.hpp
         public/aoc_lib/string.hpp
  PRIVATE src/input.cpp src/string.cpp)

target_include_directories(aoc_lib PUBLIC public/)

target_link_libraries(aoc_lib CLI11)

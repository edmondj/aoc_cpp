add_library(aoc_main)

target_sources(
  aoc_main
  PUBLIC public/aoc_main/main.hpp
  PRIVATE src/main.cpp)

target_include_directories(aoc_main PUBLIC public/)

target_link_libraries(aoc_main PUBLIC aoc_lib)

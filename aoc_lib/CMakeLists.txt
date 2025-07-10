add_library(aoc_lib)

find_package(CLI11 REQUIRED)

target_sources(
  aoc_lib
  PUBLIC public/aoc_lib/geometry/algorithm.hpp
         public/aoc_lib/geometry/dimensions.hpp
         public/aoc_lib/geometry/dyn_matrix.hpp
         public/aoc_lib/geometry/fixed_matrix.hpp
         public/aoc_lib/geometry/fixed_matrix_format.hpp
         public/aoc_lib/geometry/point.hpp
         public/aoc_lib/geometry/point_format.hpp
         public/aoc_lib/geometry/scalar.hpp
         public/aoc_lib/geometry/vector.hpp
         public/aoc_lib/algorithm.hpp
         public/aoc_lib/day_trait.hpp
         public/aoc_lib/geometry.hpp
         public/aoc_lib/geometry_format.hpp
         public/aoc_lib/hash.hpp
         public/aoc_lib/input.hpp
         public/aoc_lib/overload.hpp
         public/aoc_lib/regex.hpp
         public/aoc_lib/string.hpp
  PRIVATE src/hash.cpp src/input.cpp src/string.cpp src/regex.cpp)

target_include_directories(aoc_lib PUBLIC public/)

target_link_libraries(aoc_lib CLI11)

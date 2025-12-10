find_or_fetch(
  Z3
  FETCH_ARGS
  GIT_REPOSITORY
  https://github.com/Z3Prover/z3
  GIT_TAG
  745087e237e669d709ae35694728a0c479e572b3 # z3-4.15.4
  CMAKE_CACHE_ARGS
  -DZ3_BUILD_LIBZ3_SHARED:BOOL=OFF)

target_include_directories(
  libz3 SYSTEM INTERFACE $<BUILD_INTERFACE:${Z3_SOURCE_DIR}/src/api/c++>)

set(DAYS
    d01
    d02
    d03
    d04
    d05
    d06
    d07
    d08
    d09
    d10)

foreach(day ${DAYS})
  set(day_libs "")
  set(day_includes "")
  if(day STREQUAL d10)
    set(day_libs "libz3")
  endif()
  add_aoc_day(2025 "${day}" SOURCES ${day}.cpp LIBRARIES ${day_libs})
endforeach()

cmake_minimum_required(VERSION 3.23)

project(aoc_2018)

option(AOC_2018_BUILD_TESTING "Build tests for the project" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(MSVC)
  add_compile_options(/W4 /WX)
endif()

include(cmake/Dependencies.cmake)
include(cmake/AocDay.cmake)

if(AOC_2018_BUILD_TESTING)
  include(CTest)
  enable_testing()
endif()

add_subdirectory(aoc_lib)
add_subdirectory(aoc_main)

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
  add_aoc_day(${day} SOURCES ${day}.cpp)
endforeach()

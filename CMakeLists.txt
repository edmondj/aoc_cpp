cmake_minimum_required(VERSION 3.23)

project(aoc LANGUAGES CXX)

option(AOC_BUILD_TESTING "Build tests for the project" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(MSVC)
  add_compile_options(/W4 /WX)
endif()

include(cmake/Dependencies.cmake)
include(cmake/AocDay.cmake)

if(AOC_BUILD_TESTING)
  include(CTest)
  enable_testing()
endif()

add_subdirectory(aoc_lib)
add_subdirectory(aoc_main)
add_subdirectory(2018)
add_subdirectory(2025)

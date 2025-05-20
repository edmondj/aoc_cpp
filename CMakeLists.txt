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

add_library(device STATIC device.hpp device.cpp)
target_link_libraries(device PRIVATE aoc_lib)

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
    d10
    d11
    d12
    d13
    d14
    d15
    d16
    d17
    d18
    d19
    d20)

foreach(day ${DAYS})
  add_aoc_day(${day} SOURCES ${day}.cpp)
endforeach()

target_link_libraries(d16_tests PUBLIC device)
target_link_libraries(d16 PUBLIC device)

target_link_libraries(d19_tests PUBLIC device)
target_link_libraries(d19 PUBLIC device)

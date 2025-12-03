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
    d20
    d21
    d22
    d23
    d24
    d25)

foreach(day ${DAYS})
  add_aoc_day(
    2018
    "${day}"
    SOURCES
    ${day}.cpp
    OUT_TARGET
    TARGET_NAME
    OUT_TEST_TARGET
    TEST_TARGET_NAME)
  list(FIND "d16;d19;d21" "${day}" DAY_INDEX)
  if(NOT DAY_INDEX EQUAL -1)
    target_link_libraries(${TARGET_NAME} PUBLIC device)
    target_link_libraries(${TEST_TARGET_NAME} PUBLIC device)
  endif()
endforeach()

target_link_libraries(2018_d16_tests PUBLIC device)
target_link_libraries(2018_d16 PUBLIC device)

target_link_libraries(2018_d19_tests PUBLIC device)
target_link_libraries(2018_d19 PUBLIC device)

target_link_libraries(2018_d21_tests PUBLIC device)
target_link_libraries(2018_d21 PUBLIC device)

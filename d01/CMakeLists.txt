# add_executable(d01)

# target_sources(d01 PRIVATE d01.cpp)

# target_link_libraries(d01 PRIVATE aoc_lib aoc_main)

# add_custom_command( TARGET d01 POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
# "${CMAKE_CURRENT_SOURCE_DIR}/input.txt" "$<TARGET_FILE_DIR:d01>/input.txt")

# if(AOC_2018_BUILD_TESTING) find_package(GTest REQUIRED)

# add_executable(d01_tests) target_sources(d01_tests PRIVATE d01.cpp)
# target_link_libraries(d01_tests PRIVATE aoc_lib gtest_main)
# target_compile_definitions(d01_tests PRIVATE -DTESTING)

# gtest_discover_tests(d01_tests) endif()

add_aoc_day(d01 SOURCES d01.cpp)

# This function creates an executable target ${_NAME}. The target will use the
# specifed sources and libraries in PRIVATE scope, as well as the aoc_main and
# aoc_lib libraries. It will also copy the input.txt file to the build directory
# if one was found. If AOC_2018_BUILD_TESTING is set, it will also add a
# ${_NAME}_test target with the same sources and libraries, but also defining
# the TESTING macro and linking gtest_main.
function(add_aoc_day _NAME)
  cmake_parse_arguments(PARSE_ARGV 1 "" "" "" "SOURCES;LIBRARIES")

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${_NAME}")

  add_executable(${_NAME})

  target_sources(${_NAME} PRIVATE ${_SOURCES})

  target_link_libraries(${_NAME} PRIVATE aoc_lib aoc_main ${_LIBRARIES})

  set(INPUT_SOURCE "${CMAKE_SOURCE_DIR}/inputs/${_NAME}.txt")
  set(INPUT_DEST "$<TARGET_FILE_DIR:${_NAME}>/input.txt")

  add_custom_command(
    TARGET ${_NAME}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} "-DSRC=${INPUT_SOURCE}" "-DDST=${INPUT_DEST}" -P
            cmake/scripts/CopyIfExists.cmake)

  if(AOC_2018_BUILD_TESTING)
    find_package(GTest REQUIRED)

    add_executable(${_NAME}_tests)
    target_sources(${_NAME}_tests PRIVATE ${_SOURCES})
    target_link_libraries(${_NAME}_tests PRIVATE aoc_lib gtest_main
                                                 ${_LIBRARIES})
    target_compile_definitions(${_NAME}_tests PRIVATE -DTESTING)

    gtest_discover_tests(${_NAME}_tests)
  endif()

endfunction()

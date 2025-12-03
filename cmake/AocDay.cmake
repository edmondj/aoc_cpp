# This function creates an executable target ${_NAME}. The target will use the
# specifed sources and libraries in PRIVATE scope, as well as the aoc_main and
# aoc_lib libraries. It will also copy the input.txt file to the build directory
# if one was found. If AOC_BUILD_TESTING is set, it will also add a
# ${_NAME}_test target with the same sources and libraries, but also defining
# the TESTING macro and linking gtest_main.
function(add_aoc_day _YEAR _NAME)
  cmake_parse_arguments(PARSE_ARGV 2 "" "" "OUT_TARGET;OUT_TEST_TARGET"
                        "SOURCES;LIBRARIES")

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${_NAME}")

  set(TARGET_NAME "${_YEAR}_${_NAME}")

  if(_OUT_TARGET)
    set("${_OUT_TARGET}"
        "${TARGET_NAME}"
        PARENT_SCOPE)
  endif()

  add_executable("${TARGET_NAME}")

  target_sources("${TARGET_NAME}" PRIVATE ${_SOURCES})

  target_link_libraries("${TARGET_NAME}" PRIVATE aoc_lib aoc_main ${_LIBRARIES})

  set(INPUT_SOURCE "${CMAKE_SOURCE_DIR}/inputs/${_YEAR}/${_NAME}.txt")
  set(INPUT_DEST "$<TARGET_FILE_DIR:${TARGET_NAME}>/input.txt")

  add_custom_command(
    TARGET ${TARGET_NAME}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} "-DSRC=${INPUT_SOURCE}" "-DDST=${INPUT_DEST}" -P
            cmake/scripts/CopyIfExists.cmake)

  if(AOC_BUILD_TESTING)
    find_package(GTest REQUIRED)

    if(_OUT_TEST_TARGET)
      set("${_OUT_TEST_TARGET}"
          "${TARGET_NAME}_tests"
          PARENT_SCOPE)
    endif()

    add_executable("${TARGET_NAME}_tests")
    target_sources("${TARGET_NAME}_tests" PRIVATE ${_SOURCES})
    target_link_libraries("${TARGET_NAME}_tests" PRIVATE aoc_lib gtest_main
                                                         ${_LIBRARIES})
    target_compile_definitions("${TARGET_NAME}_tests" PRIVATE -DTESTING)

    gtest_discover_tests("${TARGET_NAME}_tests" NO_PRETTY_VALUES)
  endif()

endfunction()

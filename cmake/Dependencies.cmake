include(FetchContent)

function(find_or_fetch PACKAGE_NAME)
  cmake_parse_arguments(PARSE_ARGV 1 "" "" "" "FETCH_ARGS;CMAKE_CACHE_ARGS")
  find_package("${PACKAGE_NAME}" QUIET)
  if(NOT ${${PACKAGE_NAME}_FOUND})
    message("Fetching ${PACKAGE_NAME}...")
    foreach(CACHE_ARG ${_CMAKE_CACHE_ARGS})
      if("${CACHE_ARG}" MATCHES "^-D(.*):(.*)=(.*)$")
        set(${CMAKE_MATCH_1}
            ${CMAKE_MATCH_3}
            CACHE ${CMAKE_MATCH_2} '' FORCE)
      else()
        message(FATAL_ERROR "Wrong CMAKE_CACHE_ARGS format: ${CACHE_ARG}")
      endif()
    endforeach()
    FetchContent_Declare(${PACKAGE_NAME} ${_FETCH_ARGS} OVERRIDE_FIND_PACKAGE)
    FetchContent_MakeAvailable(${PACKAGE_NAME})
    message("Fetched ${PACKAGE_NAME}")
  else()
    message("${PACKAGE_NAME} found")
  endif()
endfunction()

# CLI11
find_or_fetch(
  CLI11
  FETCH_ARGS
  GIT_REPOSITORY
  https://github.com/CLIUtils/CLI11.git
  GIT_TAG
  4160d259d961cd393fd8d67590a8c7d210207348 # 2.5.0
  CMAKE_CACHE_ARGS
  -DCLI11_BUILD_DOCS:BOOL=OFF
  -DCLI11_BUILD_EXAMPLES:BOOL=OFF)

find_or_fetch(
  GTest
  FETCH_ARGS
  GIT_REPOSITORY
  https://github.com/google/googletest.git
  GIT_TAG
  6910c9d9165801d8827d628cb72eb7ea9dd538c5 # 1.16.0
  CMAKE_CACHE_ARGS
  -DINSTALL_GTEST:BOOL=OFF)

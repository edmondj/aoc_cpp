set(DAYS
    d01
    d02
    d03
    d04
    d05
    d06
    d07)

foreach(day ${DAYS})
  add_aoc_day(2025 "${day}" SOURCES ${day}.cpp)
endforeach()

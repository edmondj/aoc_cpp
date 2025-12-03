set(DAYS d01 d02)

foreach(day ${DAYS})
  add_aoc_day(2025 "${day}" SOURCES ${day}.cpp)
endforeach()

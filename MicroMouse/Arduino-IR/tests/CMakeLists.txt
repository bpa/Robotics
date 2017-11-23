include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

set(TEST_SOURCES
  check_runs.cpp
  check_queue.cpp
  check_astar.cpp
  maze.cpp
  main.cpp
)

add_executable(check_mouse ${TEST_SOURCES})
target_link_libraries(check_mouse micromouse ${CHECK_LIBRARIES})

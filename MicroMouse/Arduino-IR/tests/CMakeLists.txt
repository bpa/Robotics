include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

set(LIB_SOURCES
  ../src/a_star.cpp
  ../src/hardware.cpp
  ../src/movement.cpp
  ../src/queue.cpp
  ../src/state.cpp
  ../src/wheel.cpp
)

set(TEST_SOURCES
  check_astar.cpp
  check_explore.cpp
  check_localization.cpp
  check_maze.cpp
  check_movement.cpp
  check_queue.cpp
  check_race_to_center.cpp
  check_return_to_start.cpp
  check_validate_path.cpp
  main.cpp
  maze.cpp
  mocks.cpp
)

add_executable(check_mouse ${TEST_SOURCES} ${LIB_SOURCES})
target_link_libraries(check_mouse micromouse ${CHECK_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(TEST_SOURCES
  check_queue.c
  check_astar.c
  main.c
)

add_executable(check_mouse ${TEST_SOURCES})
target_link_libraries(check_mouse micromouse Threads::Threads ${CHECK_LIBRARIES})
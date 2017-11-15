cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)
project(micromouse)

find_package(gazebo REQUIRED)
include_directories(${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")

set(LIB_SOURCES
  a_star.c
  mouse.c
)

set(HEADERS 
  ${CONFIG_HEADER}
  a_star.h
  mouse.h
  queue.h
)

add_library(micromouse STATIC ${LIB_SOURCES} ${HEADERS})
target_link_libraries(micromouse ${GAZEBO_libraries})
add_library(micromouse_plugin SHARED plugin.cc ${LIB_SOURCES} ${HEADERS})

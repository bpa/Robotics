cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)
project(micromouse)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(LIB_SOURCES
  a_star.cpp
  hardware.cpp
  movement.cpp
  queue.cpp
  state.cpp
  wheel.cpp
)

add_library(micromouse STATIC ${LIB_SOURCES})

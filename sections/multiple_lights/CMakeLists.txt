CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(Multiple-lights)

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../../cmake/packages.cmake)

FOREACH(prog multiple_lights)
  ADD_EXECUTABLE(${prog} ${CMAKE_CURRENT_LIST_DIR}/src/${prog}.cpp)
ENDFOREACH()

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../../cmake/after_exe.cmake)

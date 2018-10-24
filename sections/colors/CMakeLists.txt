CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(colors)

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../../cmake/packages.cmake)

FOREACH(prog lighting_scene)
  ADD_EXECUTABLE(${prog} ${CMAKE_CURRENT_LIST_DIR}/src/${prog}.cpp)
ENDFOREACH()

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../../cmake/after_exe.cmake)

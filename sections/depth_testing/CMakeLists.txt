CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(Depth-testing)

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../../cmake/packages.cmake)

FOREACH(prog depth_testing depth_buffer depth_testing_view)
  ADD_EXECUTABLE(${prog} ${CMAKE_CURRENT_LIST_DIR}/src/${prog}.cpp)
ENDFOREACH()

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../../cmake/after_exe.cmake)

CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(Face-culling)

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../../cmake/packages.cmake)

FOREACH(prog cube)
  ADD_EXECUTABLE(${prog} ${CMAKE_CURRENT_LIST_DIR}/src/${prog}.cpp)
ENDFOREACH()

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../../cmake/after_exe.cmake)

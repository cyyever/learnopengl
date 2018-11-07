CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(Framebuffers)

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../../cmake/packages.cmake)

FOREACH(prog frame_buffer inversion grayscale sharpen blur edge)
  ADD_EXECUTABLE(${prog} ${CMAKE_CURRENT_LIST_DIR}/src/${prog}.cpp)
ENDFOREACH()

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../../cmake/after_exe.cmake)

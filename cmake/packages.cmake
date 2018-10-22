INCLUDE(${CMAKE_CURRENT_LIST_DIR}/modules/all.cmake)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)
FIND_PACKAGE(glm REQUIRED)

FIND_PROGRAM (glad_binary glad)
IF(NOT glad_binary)
  message(FATAL_ERROR "no glad found")
ENDIF()

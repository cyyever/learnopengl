CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(hello_triangle)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/all.cmake)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)
FIND_PACKAGE(glm REQUIRED)

FIND_PROGRAM (glad_binary glad)
IF(NOT glad_binary)
  message(FATAL_ERROR "no glad found")
ENDIF()

SET(glad_DIR ${CMAKE_CURRENT_BINARY_DIR}/glad)

ADD_CUSTOM_COMMAND(OUTPUT ${glad_DIR}/src/glad.c COMMAND ${glad_binary} --reproducible --profile core --generator=c --spec gl --out-path=${glad_DIR})


FOREACH(prog rotation)
  ADD_EXECUTABLE(${prog} ${CMAKE_CURRENT_SOURCE_DIR}/src/${prog}.cpp ${glad_DIR}/src/glad.c)
  TARGET_LINK_LIBRARIES(${prog} PRIVATE OpenGL::GL glfw ${CMAKE_DL_LIBS} glm)
  TARGET_INCLUDE_DIRECTORIES(${prog} PRIVATE ${glad_DIR}/include)
  add_custom_command(TARGET ${prog} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../resource ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/resource
    )
ENDFOREACH()

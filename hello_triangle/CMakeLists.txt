CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(hello_triangle)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/all.cmake)

SET(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
FILE(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)
FIND_PROGRAM (glad_binary glad)

SET(glad_DIR ${CMAKE_CURRENT_BINARY_DIR}/glad)

ADD_CUSTOM_COMMAND(OUTPUT ${glad_DIR}/src/glad.c COMMAND ${glad_binary} --profile core --generator=c --spec gl --out-path=${glad_DIR})

LIST(APPEND SOURCES ${glad_DIR}/src/glad.c)

ADD_EXECUTABLE(triangle ${SOURCES})


TARGET_LINK_LIBRARIES(triangle PRIVATE OpenGL::GL glfw ${CMAKE_DL_LIBS})

TARGET_INCLUDE_DIRECTORIES(triangle PRIVATE ${glad_DIR}/include)

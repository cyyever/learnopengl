CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(hello_triangle)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/all.cmake)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)
FIND_PROGRAM (glad_binary glad)

SET(glad_DIR ${CMAKE_CURRENT_BINARY_DIR}/glad)

ADD_CUSTOM_COMMAND(OUTPUT ${glad_DIR}/src/glad.c COMMAND ${glad_binary} --profile core --generator=c --spec gl --out-path=${glad_DIR})

ADD_EXECUTABLE(triangle ${CMAKE_CURRENT_SOURCE_DIR}/src/triangle.cpp ${glad_DIR}/src/glad.c)
TARGET_LINK_LIBRARIES(triangle PRIVATE OpenGL::GL glfw ${CMAKE_DL_LIBS})
TARGET_INCLUDE_DIRECTORIES(triangle PRIVATE ${glad_DIR}/include)

ADD_EXECUTABLE(rectangle ${CMAKE_CURRENT_SOURCE_DIR}/src/rectangle.cpp ${glad_DIR}/src/glad.c)
TARGET_LINK_LIBRARIES(rectangle PRIVATE OpenGL::GL glfw ${CMAKE_DL_LIBS})
TARGET_INCLUDE_DIRECTORIES(rectangle PRIVATE ${glad_DIR}/include)

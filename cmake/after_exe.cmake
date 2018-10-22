SET(glad_DIR ${CMAKE_CURRENT_BINARY_DIR}/glad)
ADD_CUSTOM_COMMAND(OUTPUT ${glad_DIR}/src/glad.c COMMAND ${glad_binary} --reproducible --profile core --generator=c --spec gl --out-path=${glad_DIR})

get_directory_property(progs BUILDSYSTEM_TARGETS)

FOREACH(prog ${progs})
  get_target_property(property_var "${prog}" TYPE)
  if(NOT property_var STREQUAL EXECUTABLE)
    continue()
  endif()

  TARGET_SOURCES(${prog} PRIVATE ${glad_DIR}/src/glad.c)
  TARGET_LINK_LIBRARIES(${prog} PRIVATE OpenGL::GL glfw ${CMAKE_DL_LIBS} glm)
  TARGET_INCLUDE_DIRECTORIES(${prog} PRIVATE ${glad_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/../include)
  add_custom_command(TARGET ${prog} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/../resource ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/resource
    )
ENDFOREACH()
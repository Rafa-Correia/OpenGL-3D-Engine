cmake_minimum_required(VERSION 3.15)

project(Engine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

add_subdirectory(src/engine)
add_subdirectory(src/external)

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OPENGL not found!")
endif()

if(WIN32)
    set(LIBRARIES_FOLDER "${CMAKE_SOURCE_DIR}/libraries")
    message(STATUS "Libraries folder has been set to: ${LIBRARIES_FOLDER}")

    include_directories(
        ${LIBRARIES_FOLDER}/glut
        ${LIBRARIES_FOLDER}/glew
        ${LIBRARIES_FOLDER}/devil
    )

    target_link_libraries(engine PRIVATE
        ${OPENGL_LIBRARIES}
        ${LIBRARIES_FOLDER}/glut/glut32.lib
        ${LIBRARIES_FOLDER}/glew/glew32.lib
        ${LIBRARIES_FOLDER}/devil/DevIL.lib
    )

    if(EXISTS "${LIBRARIES_FOLDER}/glut/glut32.dll" AND EXISTS "${LIBRARIES_FOLDER}/glew/glew32.dll" AND EXISTS "${LIBRARIES_FOLDER}/devil/devil.dll")
        file(COPY "${LIBRARIES_FOLDER}/glut/glut32.dll" DESTINATION ${CMAKE_BINARY_DIR})
        file(COPY "${LIBRARIES_FOLDER}/glew/glew32.dll" DESTINATION ${CMAKE_BINARY_DIR})
        file(COPY "${LIBRARIES_FOLDER}/devil/devil.dll" DESTINATION ${CMAKE_BINARY_DIR})
    else()
        message(WARNING "Binary (.dll) of one or more of the libraries is missing. The program might not be able to function.")
        if(EXISTS "${LIBRARIES_FOLDER}/glut/glut32.dll")
            message(STATUS "\t-glut")
        endif()
        if(EXISTS "${LIBRARIES_FOLDER}/glew/glew32.dll")
            message(STATUS "\t-glew")
        endif()
        if(EXISTS "${LIBRARIES_FOLDER}/devil/devil.dll")
            message(STATUS "\t-devil")
        endif()
    endif()
else()
    message(WARNING "Building isn't configured for UNIX systems.")
endif()
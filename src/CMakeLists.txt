file(GLOB_RECURSE ENGINE_SOURCES *.cpp)
add_executable(engine ${ENGINE_SOURCES})
target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/include)


# i dont think this is needed?
# im linking everything in the main CMakeLists file

if (WIN32)
    target_include_directories(engine PRIVATE
        ${CMAKE_SOURCE_DIR}/libraries/glut
        ${CMAKE_SOURCE_DIR}/libraries/glew
        ${CMAKE_SOURCE_DIR}/libraries/devil
    )
endif()
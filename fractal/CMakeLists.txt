cmake_minimum_required(VERSION 3.5)
project(fractal)

set(CMAKE_CXX_STANDARD 11)
find_package(GLEW REQUIRED)
find_package(glfw3 3.1.2 REQUIRED)
add_custom_command(
        OUTPUT ../3rdparty/glm
        COMMAND unzip glm-0.9.8.5.zip
        WORKING_DIRECTORY ../3rdparty
        COMMENT "Unpacking glm"
)

add_subdirectory(../3rdparty/AntTweakBar ../3rdparty/AntTweakBar/lib)
set_target_properties(AntTweakBar PROPERTIES LINKER_LANGUAGE CXX)


AUX_SOURCE_DIRECTORY(src/ SOURCE_FILES)
set(SOURCE_FILES ${COMMON_FILES} ${SOURCE_FILES} )

add_executable(fractal
        ../3rdparty/glm
        ${SOURCE_FILES}
        texture.fsh
        transform.vsh
        ../common/shader.h ../common/shader.cpp
        ../common/glfw_window_manager.h ../common/glfw_window_manager.cpp
)

include_directories(src ../common ../3rdparty/AntTweakBar/include ../3rdparty/glm)

target_link_libraries(fractal -lGLEW -lGLU -lGL)
target_link_libraries(fractal glfw)
target_link_libraries(fractal AntTweakBar)

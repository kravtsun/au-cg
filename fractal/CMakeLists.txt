cmake_minimum_required(VERSION 3.5)
project(fractal)

set(CMAKE_CXX_STANDARD 11)
find_package(GLEW REQUIRED)
find_package(glfw3 3.1.2 REQUIRED)

#include(../3rdparty/AntTweakBar/CMakeLists.txt)
#add_library(AntTweakBar STATIC ../3rdparty/AntTweakBar)
add_subdirectory(../3rdparty/AntTweakBar ../3rdparty/AntTweakBar/lib)
set_target_properties(AntTweakBar PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(../3rdparty/glm ../3rdparty/glm/include)


AUX_SOURCE_DIRECTORY(src/ SOURCE_FILES)

add_executable(fractal ${SOURCE_FILES}
    texture.fsh
    transform.vsh
)

include_directories(src ../3rdparty/AntTweakBar/include ../3rdparty/glm)

target_link_libraries(fractal -lGLEW -lGLU -lGL)
target_link_libraries(fractal glfw)
target_link_libraries(fractal AntTweakBar)

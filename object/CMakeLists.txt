cmake_minimum_required(VERSION 3.5)
project(object)

set(CMAKE_CXX_STANDARD 11)
find_package(GLEW REQUIRED)
find_package(glfw3 3.1.2 REQUIRED)
find_package(assimp 3.2 REQUIRED)

file(GLOB COMMON_FILES
    ../common/*.cpp
    ../common/*.h
)

AUX_SOURCE_DIRECTORY(src/ SOURCE_FILES)
set(SOURCE_FILES ${COMMON_FILES} ${SOURCE_FILES})

add_executable(object
    ../3rdparty/glm
    ${SOURCE_FILES}
    shader.frag
    shader.vert
)

include_directories(src ../common ../3rdparty/AntTweakBar/include ../3rdparty/glm)

target_link_libraries(object -lassimp)
target_link_libraries(object -lGLEW -lGLU -lGL)
target_link_libraries(object glfw)

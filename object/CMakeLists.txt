cmake_minimum_required(VERSION 3.9)
project(object)

set(CMAKE_CXX_STANDARD 11)
find_package(GLEW REQUIRED)
find_package(glfw3 3.1.2 REQUIRED)

AUX_SOURCE_DIRECTORY(../common/ COMMON_FILES)
AUX_SOURCE_DIRECTORY(src/ SOURCE_FILES)
set(SOURCE_FILES ${COMMON_FILES} ${SOURCE_FILES} )

include_directories(src ../common ../3rdparty/glm)

add_executable(object
            ../3rdparty/glm
            ${SOURCE_FILES}
            fragment_shader.fsh
            vertex_shader.vsh)

target_link_libraries(object -lGLEW -lGLU -lGL)
target_link_libraries(object glfw)

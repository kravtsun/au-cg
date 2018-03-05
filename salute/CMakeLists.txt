cmake_minimum_required(VERSION 3.5)
project(salute)

# Setting old compiler as 6.3 gets to C++-17 standard despite 'set(CMAKE_CXX_STANDARD 11)'
#set(CMAKE_C_COMPILER "/usr/bin/gcc-4.8")
#set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.8")

#set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_STANDARD 11)
find_package(GLEW REQUIRED)
find_package(glfw3 3.1.2 REQUIRED)

EXECUTE_PROCESS(COMMAND pwd)

add_custom_command(
    OUTPUT ../3rdparty/glm
    COMMAND unzip glm-0.9.8.5.zip
    WORKING_DIRECTORY ../3rdparty
    COMMENT "Unpacking glm"
)

file(GLOB COMMON_FILES
    ../common/shader.*
    ../common/glfw_window_manager.*
)

AUX_SOURCE_DIRECTORY(src/ SOURCE_FILES)
AUX_SOURCE_DIRECTORY(src/wrappers WRAPPER_FILES)
set(WRAPPER_FILES ${WRAPPER_FILES} src/wrappers/AllWrappers.h)
set(SOURCE_FILES ${COMMON_FILES} ${SOURCE_FILES} ${WRAPPER_FILES} src/wrappers/VertexArray.cpp src/wrappers/VertexArray.h src/wrappers/VertexBuffer.cpp src/wrappers/VertexBuffer.h)

add_executable(salute
    ../3rdparty/glm
    ${SOURCE_FILES}
)

include_directories(src ../common ../3rdparty/glm)

target_link_libraries(salute -lGLEW -lGLU -lGL)
target_link_libraries(salute glfw)

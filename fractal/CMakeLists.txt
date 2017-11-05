cmake_minimum_required(VERSION 3.5)
project(fractal)

set(CMAKE_CXX_STANDARD 11)


set(SOURCE_FILES src/main.cpp src/shader.cpp src/shader.hpp src/texture_loader.cpp src/texture_loader.h)
#set(INCLUDE_FILES src/shader.hpp)
add_executable(fractal ${SOURCE_FILES}
    texture.fsh
    transform.vsh
)

include_directories(src)

#add_library(glew SHARED IMPORTED)
#add_library(glfw3 SHARED IMPORTED)

target_link_libraries(fractal -lGLEW -lGLU -lGL)

target_link_libraries(fractal glfw)


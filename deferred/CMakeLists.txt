cmake_minimum_required(VERSION 3.5)
project(deferred)

# Setting old compiler as 6.3 gets to C++-17 standard despite 'set(CMAKE_CXX_STANDARD 11)'
#set(CMAKE_C_COMPILER "/usr/bin/gcc-4.8")
#set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.8")

#set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_STANDARD 11)
find_package(GLEW REQUIRED)
find_package(glfw3 3.1.2 REQUIRED)
find_package(assimp 3.2 REQUIRED)

add_custom_command(
    OUTPUT ../3rdparty/glm
    COMMAND unzip glm-0.9.8.5.zip
    WORKING_DIRECTORY ../3rdparty
    COMMENT "Unpacking glm"
)

add_subdirectory(../3rdparty/AntTweakBar ../3rdparty/AntTweakBar/lib)
set_target_properties(AntTweakBar PROPERTIES LINKER_LANGUAGE CXX)

file(GLOB COMMON_FILES
    ../common/*.cpp
    ../common/*.h
)
AUX_SOURCE_DIRECTORY(src/ SOURCE_FILES)
set(SOURCE_FILES ${COMMON_FILES} ${SOURCE_FILES}
    src/GeometryPass.cpp src/GeometryPass.h
    src/PassCommon.h src/PassCommon.cpp
    src/LightPass.cpp src/LightPass.h
    src/TexturePass.cpp src/TexturePass.h
    src/Light.h
    src/ThresholdPass.cpp src/ThresholdPass.h
    src/BlurPass.cpp src/BlurPass.h
)

add_executable(deferred
    ../3rdparty/glm
    ${SOURCE_FILES}
    geom.vsh geom.fsh
    light.fsh PassthroughTexture.vsh
    debug.fsh threshold.fsh
    blur.fsh
)

include_directories(src ../common ../3rdparty/AntTweakBar/include ../3rdparty/glm)

target_link_libraries(deferred -lassimp)
target_link_libraries(deferred -lGLEW -lGLU -lGL)
target_link_libraries(deferred glfw)
target_link_libraries(deferred AntTweakBar)

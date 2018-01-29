cmake_minimum_required(VERSION 3.5)
project(deferred)

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
set(SOURCE_FILES ${COMMON_FILES} ${SOURCE_FILES})

add_executable(deferred
    ../3rdparty/glm
    ${SOURCE_FILES}
    geom.vsh geom.fsh
    light.fsh light.vsh
)

include_directories(src ../common ../3rdparty/AntTweakBar/include ../3rdparty/glm)

target_link_libraries(deferred -lassimp)
target_link_libraries(deferred -lGLEW -lGLU -lGL)
target_link_libraries(deferred glfw)
target_link_libraries(deferred AntTweakBar)

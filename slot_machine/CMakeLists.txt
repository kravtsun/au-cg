cmake_minimum_required(VERSION 3.5)
project(slot_machine)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-ggdb -pthread")

find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glm
        COMMAND unzip glm-0.9.8.5.zip
        WORKING_DIRECTORY ../3rdparty
        COMMENT "Unpacking glm"
)

file(GLOB COMMON_FILES
        ../common/shader.*
        ../common/window_manager.*
        ../common/glut_window_manager.*
        ../common/AbstractPass.*
        ../common/PassthroughPass.*
)

file(GLOB WRAPPER_FILES
        ../common/wrappers/*.h
        ../common/wrappers/*.cpp
)

#AUX_SOURCE_DIRECTORY(src/ SOURCE_FILES)
set(SOURCE_FILES
        src/main.cpp
        src/gl_holder.h src/gl_holder.cpp
        src/color/ColorSupplier.h
        src/color/BlinkColor.h src/color/BlinkColor.cpp
        src/color/ConstantColor.h src/color/ConstantColor.cpp
        src/Button.h src/Button.cpp
        src/StartButton.h src/StartButton.cpp
        src/StartButtonPass.h src/StartButtonPass.cpp
        src/CellsPass.h src/CellsPass.cpp
        src/CellHolder.h src/CellHolder.cpp
)

set(SOURCE_FILES ${COMMON_FILES} ${SOURCE_FILES} ${WRAPPER_FILES} src/StartButton.cpp src/StartButton.h src/color/BlinkColor.cpp src/color/BlinkColor.h src/color/ColorSupplier.h src/color/ConstantColor.cpp src/color/ConstantColor.h src/resources.h)

add_executable(slot_machine
        ../3rdparty/glm
        ${SOURCE_FILES}
)

include_directories(src ../common ../3rdparty/glm)
target_link_libraries(slot_machine -lGLEW -lGL -lglut)

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glfw_window_manager.h"

volatile int gl_error;

static void init_glew() {
    glewExperimental = GL_TRUE; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        throw std::logic_error("Failed to initialize GLEW");
    }
    gl_error = glGetError(); // can give gl_error != GL_NO_ERROR after glewInit.
}

GLFWWindowManager::GLFWWindowManager(const std::string &win_name, const int win_width, const int win_height) :
        win_name_(win_name), win_width_(win_width), win_height_(win_height) {
    if (win_width * win_height == 0) {
        throw std::logic_error("Window is empty");
    }
    // Initialise GLFW
    if (!glfwInit()) {
        throw std::logic_error("Failed to initialize GLFW\n");
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Open a window and create its OpenGL context
    window_ = glfwCreateWindow(win_width, win_height, win_name.c_str(), nullptr, nullptr);
    
    if (window_ == nullptr) {
        throw std::logic_error("Failed to open GLFW window. ");
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);
//    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    
    init_glew();
}

void GLFWWindowManager::main_loop(std::function<void()> inside_action) const {
    // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window_, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window_)) {
        inside_action();
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

void GLFWWindowManager::resize(int new_width, int new_height) {
    win_width_ = new_width;
    win_height_ = new_height;
}

GLFWWindowManager::~GLFWWindowManager() {
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}

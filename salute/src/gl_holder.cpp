#include <memory>
#include <cassert>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "glfw_window_manager.h"
#include "gl_holder.h"

GLHolder::GLHolder(std::shared_ptr<GLFWWindowManager> window_manager)
        : window_manager(window_manager)
        , frame_pass(width(), height())
        , frame_combiner_pass(width(), height())
        , passthrough_pass(width(), height())
{
    assert(glGetError() == GL_NO_ERROR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width(), height());
    glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLHolder::paint() {
    static bool isPaused = false;
    if (glfwGetKey(window_manager->window(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        isPaused = !isPaused;
    }
    if (isPaused) {
        return;
    }
    
    if (glfwGetKey(window_manager->window(), GLFW_KEY_ENTER) == GLFW_PRESS) {
        frame_pass.reset();
        frame_combiner_pass.reset();
    }
    
    frame_pass.pass();
    frame_combiner_pass.set_input_texture(frame_pass.output_texture());
    frame_combiner_pass.pass();
    passthrough_pass.set_input_texture(frame_combiner_pass.output_texture());
    passthrough_pass.pass();
    
    assert(glGetError() == GL_NO_ERROR);
}

GLHolder::~GLHolder() {
}

int GLHolder::width() const {
    return window_manager->win_width();
}

int GLHolder::height() const {
    return window_manager->win_height();
}


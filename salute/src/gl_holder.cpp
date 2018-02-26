#include <memory>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "gl_holder.h"

#include "shader.h"
#include <GLFW/glfw3.h>

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
    if (glfwGetKey(window_manager->window(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        frame_pass.isPaused = !frame_pass.isPaused;
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


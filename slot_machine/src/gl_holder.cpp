#include <cassert>

#include "gl_holder.h"
#include "glfw_window_manager.h"

GLHolder::GLHolder(std::shared_ptr<GLFWWindowManager> window_manager)
        : window_manager(window_manager)
        , passthrough_pass(width(), height())
        , cells_pass(width(), height())
{
}

int GLHolder::width() const {
    return window_manager->win_width();
}

int GLHolder::height() const {
    return window_manager->win_height();
}

void GLHolder::paint() {
    cells_pass.pass();
    passthrough_pass.set_input_texture(cells_pass.output_texture());
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width(), height());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    passthrough_pass.pass();
    assert(glGetError() == GL_NO_ERROR);
}

GLHolder::~GLHolder() = default;

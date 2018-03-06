#include <cassert>

#include "gl_holder.h"
#include "glfw_window_manager.h"

#define PHI 1.6180339887

GLHolder::GLHolder(std::shared_ptr<GLFWWindowManager> window_manager)
        : window_manager(window_manager)
        , passthrough_pass(width(), height())
{
    std::vector<std::vector<std::string>> cells_lines(5);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 5; ++j) {
            std::string value = "x";
            value += static_cast<char>('0' + i);
            value += static_cast<char>('1' + j);
            cells_lines[j].push_back(value);
        }
    }
    
    const int h = 200;
    const glm::vec2 roi_size{static_cast<int>(h * PHI) / 2, h};
    glm::vec2 roi_pos{30, 350};
    const int cell_padding = 30;
    for (auto const &lines : cells_lines) {
        cell_passes.emplace_back(roi_pos, roi_size, lines);
        cell_passes.back().set_yspeed((float)rand() / RAND_MAX / 1000);
        roi_pos.x += roi_size.x + cell_padding;
    }
    CellPass::prepare_passthrough(width(), height());
}

int GLHolder::width() const {
    return window_manager->win_width();
}

int GLHolder::height() const {
    return window_manager->win_height();
}

void GLHolder::paint() {
    CellPass::prepare_pass();
    for (auto &cell_pass : cell_passes) {
        cell_pass.pass();
    }
    passthrough_pass.set_input_texture(CellPass::output_texture());
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width(), height());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    passthrough_pass.pass();
    assert(glGetError() == GL_NO_ERROR);
}

GLHolder::~GLHolder() {

}

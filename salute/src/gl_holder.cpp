#include <memory>
#include <cassert>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "glfw_window_manager.h"
#include "gl_holder.h"

GLHolder::GLHolder(std::shared_ptr<GLFWWindowManager> window_manager)
        : window_manager(window_manager)
        , salutes_combiner_pass(width(), height())
        , passthrough_pass(width(), height())
        , sky_pass(width(), height(), "black_clouds.bmp")
{
    assert(glGetError() == GL_NO_ERROR);
}

void GLHolder::add_salute(double x, double y) {
    x = (x / width() - 0.5) * 2 * FramePass::limit;
    y = (0.5 - y / height()) * 2 * FramePass::limit;
    const glm::vec3 position{x, y, 0};
    glm::vec3 color{1, 1, 1};
    for (int i = 0; i < 3; ++i) {
        color[i] = static_cast<float>(rand()) / RAND_MAX;
    }
    const int nparticles = (rand() % 10 + 1) * 100;
    const float speed_magnitude = (rand() % 10 + 3) * 5.f;
    salutes.push_back(std::make_shared<SalutePass>(width(), height(), position, color, nparticles, speed_magnitude));
}

void GLHolder::paint() {
    static bool is_paused = false;
    if (glfwGetKey(window_manager->window(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        is_paused = !is_paused;
    }
    if (is_paused) {
        return;
    }
    
    if (glfwGetKey(window_manager->window(), GLFW_KEY_ENTER) == GLFW_PRESS) {
        for (auto &salute : salutes) {
            salute->reset();
        }
    }

    sky_pass.pass();
    salutes_combiner_pass.reset();
    salutes_combiner_pass.set_input_texture(sky_pass.output_texture());
    salutes_combiner_pass.pass();
    for (auto it = salutes.begin(); it != salutes.end(); ) {
        auto &salute = *it;
        if (!salute->is_alive()) {
            it = salutes.erase(it);
        } else {
            salute->pass();
            salutes_combiner_pass.set_input_texture(salute->output_texture());
            salutes_combiner_pass.pass();
            ++it;
        }
    }
    passthrough_pass.set_input_texture(salutes_combiner_pass.output_texture());
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width(), height());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    passthrough_pass.pass();
    assert(glGetError() == GL_NO_ERROR);
}

GLHolder::~GLHolder() = default;

int GLHolder::width() const {
    return window_manager->win_width();
}

int GLHolder::height() const {
    return window_manager->win_height();
}

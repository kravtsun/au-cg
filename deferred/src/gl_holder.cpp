#include <memory>
#include <cassert>
#include <glm/glm.hpp>
#include "gl_holder.h"
#include "controls.h"
#include "Light.h"

static std::string hex(int num) {
    std::string res = "";
    if (num == 0) {
        return "0";
    } else {
        bool has_minus = num < 0;
        num = std::abs(num);
        while (num > 0) {
            int digit = num % 16;
            if (digit >= 10) {
                res += 'A' + (digit - 10);
            } else {
                res += '0' + digit;
            }
            num /= 16;
        }
        std::reverse(res.begin(), res.end());
        if (has_minus) {
            res = "-" + res;
        }
        return res;
    }
}

GLHolder::GLHolder(std::shared_ptr<GLFWWindowManager> window_manager)
        : window_manager(window_manager)
        , geometry_pass(window_manager->win_width(), window_manager->win_height())
        , light_pass(window_manager->win_width(), window_manager->win_height())
        , texture_pass(window_manager->win_width(), window_manager->win_height())
        , threshold_pass(window_manager->win_width(), window_manager->win_height())
        , blur_pass(window_manager->win_width(), window_manager->win_height())
        , additive_pass(window_manager->win_width(), window_manager->win_height())
{
    assert(glGetError() == GL_NO_ERROR);
    glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
    geometry_pass.load_scene("sagar.obj");
    
    lights.push_back(std::make_shared<StationaryLight>());
    lights.push_back(std::make_shared<LissajousLight>(1, 2));
    lights.back()->setColor(glm::vec3(0.0, 1.0, 0.0));
    lights.push_back(std::make_shared<LissajousLight>(2, 1));
    lights.back()->setColor(glm::vec3(0.0, 0.0, 1.0));
    lights.push_back(std::make_shared<LissajousLight>(3, 2));
    lights.back()->setColor(glm::vec3(0.0, 1.0, 1.0));
    lights.push_back(std::make_shared<LissajousLight>(2, 3));
    lights.back()->setColor(glm::vec3(1.0, 1.0, 0.0));
    lights.push_back(std::make_shared<LissajousLight>(5, 4));
    lights.back()->setColor(glm::vec3(1.0, 0.0, 1.0));
    lights.push_back(std::make_shared<LissajousLight>(4, 5));
    lights.back()->setColor(glm::vec3(1.0, 0.0, 0.0));
    lights.push_back(std::make_shared<LissajousLight>(5, 6));
    lights.push_back(std::make_shared<LissajousLight>(6, 5));
    lights.push_back(std::make_shared<LissajousLight>(9, 8));
    lights.push_back(std::make_shared<LissajousLight>(8, 9));
}

void GLHolder::paint() {
    computeMatricesFromInputs(window_manager->window());
    geometry_pass.pass();
    assert(glGetError() == GL_NO_ERROR);
    
    // TODO geometry_pass's depthTexture not rendered with texture_pass.
    auto current_lights = lights;
    current_lights.resize(lights_count);
    // TODO use a separate texture_pass for light_pass's pass.
    light_pass.pass(geometry_pass, texture_pass, current_lights);
    assert(glGetError() == GL_NO_ERROR);

    if (!pause) {
        for (auto const &light : lights) {
            light->step();
        }
    }

    GLuint result_texture;
    if (bloom_is_on) {
        threshold_pass.pass(light_pass.outputTexture(), bloom_threshold);
        blur_pass.pass(threshold_pass.outputTexture());
        additive_pass.pass(light_pass.outputTexture(), blur_pass.outputTexture());
        result_texture = additive_pass.outputTexture();
    } else {
        result_texture = light_pass.outputTexture();
    }
    
    // TODO move framebuffer binding to texture_pass.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window_manager->win_width(), window_manager->win_height());
//    texture_pass.pass(threshold_pass.mask_texture);
    texture_pass.pass(result_texture);
    assert(glGetError() == GL_NO_ERROR);
}

GLHolder::~GLHolder() = default;


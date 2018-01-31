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
        , geometryPass(window_manager->win_width(), window_manager->win_height())
        , lightPass(window_manager->win_width(), window_manager->win_height())
        , texturePass(window_manager->win_width(), window_manager->win_height())
        , thresholdPass(window_manager->win_width(), window_manager->win_height())
        , blurPass(window_manager->win_width(), window_manager->win_height())
{
    assert(glGetError() == GL_NO_ERROR);
    glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
    geometryPass.load_scene("sagar.obj");
    
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
    geometryPass.pass();
    assert(glGetError() == GL_NO_ERROR);
    
    // TODO geometryPass's depthTexture not rendered with texturePass.
    auto currentLights = lights;
    currentLights.resize(lightsCount);
    // TODO use a separate texturePass for lightPass's pass.
    lightPass.pass(geometryPass, texturePass, currentLights);
    assert(glGetError() == GL_NO_ERROR);
    
    thresholdPass.pass(lightPass.color_texture, 0.7);
    blurPass.pass(thresholdPass.mask_texture);
    
    // TODO move framebuffer binding to texturePass.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window_manager->win_width(), window_manager->win_height());
//    texturePass.pass(thresholdPass.mask_texture);
    texturePass.pass(blurPass.outputTexture());
}

GLHolder::~GLHolder() = default;


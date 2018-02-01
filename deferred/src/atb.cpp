#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>
#include <glm/glm.hpp>

#include "atb.h"
#include "controls.h"
#include "glfw_window_manager.h"
#include "gl_holder.h"

namespace atb {
template<typename T>
static void debug_point(const glm::tvec2<T> &p, const std::string &debug_string) {
    if (!debug_string.empty()) {
        std::cout << debug_string << " = ";
    }
    std::cout << p.x << ' ' << p.y << std::endl;
}

namespace impl {
static TwBar *myBar;
static std::shared_ptr<GLFWWindowManager> window_manager;
static std::shared_ptr<GLHolder> gl_holder;

//*  @param[in] window The window that received the event.
//*  @param[in] button The[mouse button](@ref buttons) that was pressed or released.
//*  @param[in] action One of `GLFW_PRESS` or `GLFW_RELEASE`.
//*  @param[in] mods Bit field describing which[modifier keys](@ref mods) were
static void glfw_mouse_button_callback(GLFWwindow *, int button, int action, int) {
    TwEventMouseButtonGLFW(button, action);
}

static glm::dvec2 mouse_pos;

static void glfw_mouse_pos_callback(GLFWwindow *_window, double xpos, double ypos) {
    if (!TwEventMousePosGLFW(static_cast<int>(xpos), static_cast<int>(ypos))) {
        const glm::dvec2 cur_pos{xpos, ypos};
    }
    mouse_pos = glm::vec2(xpos, ypos);
}

//*  @param[in] window The window that received the event.
//*  @param[in] xoffset The scroll offset along the x - axis.
//*  @param[in] yoffset The scroll offset along the y - axis.
static void glfw_mouse_wheel_callback(GLFWwindow *, double xoffset, double yoffset) {
    if (!TwEventMouseWheelGLFW(static_cast<int>(yoffset))) {
        assert(xoffset == 0.0);
        changeFov(static_cast<float>(yoffset * 2));
    }
}

//*  @param[in] window The window that received the event.
//*  @param[in] key The[keyboard key](@ref keys) that was pressed or released.
//*  @param[in] scancode The system - specific scancode of the key.
//*  @param[in] action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`.
//*  @param[in] mods Bit field describing which[modifier keys](@ref mods) were
static void glfw_key_callback(GLFWwindow *_window, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(_window, true);
        return;
    }
    
    TwEventKeyGLFW(key, action);
}

static void glfw_char_callback(GLFWwindow *, unsigned int input_char) {
    int glfw_action = GLFW_PRESS;
    TwEventCharGLFW(input_char, glfw_action);
}

static void glfw_resize_callback(GLFWwindow *, int width, int height) {
    window_manager->resize(width, height);
    
    TwWindowSize(width, height);
    glViewport(0, 0, width, height);
}
}

void init(std::shared_ptr<GLFWWindowManager> window_manager, std::shared_ptr<GLHolder> gl_holder) {
    impl::window_manager = window_manager;
    impl::gl_holder = gl_holder;
    int tw_error;
    auto throw_on_atb_error = [&tw_error](const std::string &probable_cause) {
        if (tw_error == 0) {
            throw std::logic_error(probable_cause + ": " + TwGetLastError());
        }
    };
    tw_error = TwInit(TW_OPENGL_CORE, nullptr);
    throw_on_atb_error("TwInit");
    
    impl::myBar = TwNewBar("TweakBar");
    tw_error = TwWindowSize(window_manager->win_width(), window_manager->win_height());
    throw_on_atb_error("TwWindowSize");

    auto lightsCount = "label='lights_count' min=0 max=" + std::to_string(GLHolder::max_lights_count);
    tw_error = TwAddVarRW(impl::myBar, "lights_count", TW_TYPE_UINT32, &gl_holder->lights_count, lightsCount.c_str());
    throw_on_atb_error("TwAddVarRw(iterations)");
    
    TwEnumVal modes[] = {
            {GLHolder::Mode::POSITION, "Position"},
            {GLHolder::Mode::NORMAL, "Normal"},
            {GLHolder::Mode::DIFFUSE, "Diffuse"},
            {GLHolder::Mode::AMBIENT, "Ambient"},
            {GLHolder::Mode::DEFERRED, "Deferred"},
            {GLHolder::Mode::BLOOM_THRESHOLD, "BloomThreshold"},
            {GLHolder::Mode::BLOOM_BLUR, "BloomBlur"},
    };
    const int modesCount = sizeof(modes) / sizeof(modes[0]);
    const TwType modeType = TwDefineEnum("ModeType", modes, modesCount);
    TwAddVarRW(impl::myBar, "Mode", modeType, &gl_holder->mode, nullptr);
    TwAddVarRW(impl::myBar, "BloomSwitch", TW_TYPE_BOOLCPP, &gl_holder->bloom_is_on, "label='Bloom switch'");
    TwAddVarRW(impl::myBar, "BloomThreshold", TW_TYPE_FLOAT, &gl_holder->bloom_threshold, "label='Bloom threshold' min=0 max=1 step=0.02");
    TwAddVarRW(impl::myBar, "BloomMultiplier", TW_TYPE_FLOAT, &gl_holder->bloom_multiplier, "label='Bloom multiplier' min=0 max=3 step=0.02");
    TwAddVarRW(impl::myBar, "PauseAnimation", TW_TYPE_BOOLCPP, &gl_holder->pause, "label='Pause animation'");

    GLFWwindow *window = window_manager->window();
    // after GLFW initialization
    // directly redirect GLFW events to AntTweakBar
    glfwSetMouseButtonCallback(window, static_cast<GLFWmousebuttonfun>(impl::glfw_mouse_button_callback));
    glfwSetCursorPosCallback(window, static_cast<GLFWcursorposfun>(impl::glfw_mouse_pos_callback));
    glfwSetScrollCallback(window, static_cast<GLFWscrollfun>(impl::glfw_mouse_wheel_callback));
    
    glfwSetKeyCallback(window, static_cast<GLFWkeyfun>(impl::glfw_key_callback));
    glfwSetCharCallback(window, static_cast<GLFWcharfun>(impl::glfw_char_callback));
    
    glfwSetWindowSizeCallback(window, static_cast<GLFWwindowsizefun>(impl::glfw_resize_callback));
}

void draw() {
    impl::gl_holder->paint();
    if (TwDraw() == 0) {
        throw std::logic_error(std::string("TwDraw: ") + TwGetLastError());
    }
}
}

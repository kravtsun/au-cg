#include <iostream>
#include <memory>
#include <unistd.h>

#include "glfw_window_manager.h"
#include "gl_holder.h"
#include <GLFW/glfw3.h>

static const double FPS = 60.0;
static const double FRAME_TIME = 1.0 / FPS;

static std::shared_ptr<GLFWWindowManager> window_manager;
static std::shared_ptr<GLHolder> gl_holder;

static glm::dvec2 mouse_pos;
static glm::dvec2 last_salute_pos;
static clock_t last_salute_timestamp;

static void glfw_mouse_pos_callback(GLFWwindow *, double xpos, double ypos) {
//    if (!TwEventMousePosGLFW(static_cast<int>(xpos), static_cast<int>(ypos))) {
//        const glm::dvec2 cur_pos{xpos, ypos};
//    }
    mouse_pos = glm::vec2(xpos, ypos);
}

static inline double get_time_elapsed(clock_t time_delta) {
    return static_cast<double>(time_delta) / CLOCKS_PER_SEC;
}

static void glfw_mouse_button_callback(GLFWwindow *, int button, int action, int) {
//    TwEventMouseButtonGLFW(button, action);
    if (get_time_elapsed(clock() - last_salute_timestamp) > 0.0001 || last_salute_pos != mouse_pos) {
        gl_holder->add_salute(mouse_pos.x, mouse_pos.y);
        last_salute_timestamp = clock();
        last_salute_pos = mouse_pos;
    }
}

int main() {
    window_manager = std::make_shared<GLFWWindowManager>("Salute", 1024, 768);
    gl_holder = std::make_shared<GLHolder>(window_manager);

    clock_t ammortization_time = 0;
    
    clock_t begin = 0, end = 0;
    
//    int cnt = 0;
    
    glfwSetMouseButtonCallback(window_manager->window(), glfw_mouse_button_callback);
    glfwSetCursorPosCallback(window_manager->window(), glfw_mouse_pos_callback);
    
    window_manager->main_loop([&]() {
        ammortization_time = end - begin;
        begin = clock();
        gl_holder->paint();
//        usleep(200*1e3);
        end = clock();
        auto elapsed_secs = get_time_elapsed(end - begin + ammortization_time);
        auto paint_time = get_time_elapsed(end - begin);
//        std::cout << paint_time * 1000 << "ms" << std::endl;
//        std::cout << cnt++ << std::endl;
        auto wait_time_in_secs = FRAME_TIME - elapsed_secs;
        auto wait_time_in_usecs = wait_time_in_secs * 1e6;
        if (wait_time_in_usecs >= 1) {
            usleep(static_cast<__useconds_t>(wait_time_in_usecs));
        }
        // TODO move delay to paint procedure.
        end = clock();
    });
    return 0;
}

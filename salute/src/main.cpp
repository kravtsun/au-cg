#include <iostream>
#include <memory>
#if __linux__
#include <unistd.h>
#elif _WIN32
#include <Windows.h>
#endif

#include "glfw_window_manager.h"
#include "gl_holder.h"
#include <GLFW/glfw3.h>
#include <ctime>

#define PERFORMANCE_TEST 0

static const double FPS = 60.0;
static const double FRAME_TIME = 1.0 / FPS;

static std::shared_ptr<GLFWWindowManager> window_manager;
static std::shared_ptr<GLHolder> gl_holder;

static glm::dvec2 mouse_pos;
static glm::dvec2 last_salute_pos;
static clock_t last_salute_timestamp;

static void glfw_mouse_pos_callback(GLFWwindow *, double xpos, double ypos) {
    mouse_pos = glm::vec2(xpos, ypos);
}

static double get_time_elapsed(clock_t time_delta) {
    return static_cast<double>(time_delta) / CLOCKS_PER_SEC;
}

static void glfw_mouse_button_callback(GLFWwindow *, int, int, int) {
    if (get_time_elapsed(clock() - last_salute_timestamp) > 0.5 || last_salute_pos != mouse_pos) {
        std::cout << "Adding salute" << std::endl;
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
    
    glfwSetMouseButtonCallback(window_manager->window(), glfw_mouse_button_callback);
    glfwSetCursorPosCallback(window_manager->window(), glfw_mouse_pos_callback);

#if PERFORMANCE_TEST
    int cnt = 0;
#endif
    window_manager->main_loop([&]() {
        ammortization_time = end - begin;
        begin = clock();
#if PERFORMANCE_TEST
        if (cnt % 5 == 0) {
            gl_holder->add_salute(rand() % 1024, rand() % 768);
        }
#endif
        gl_holder->paint();
        end = clock();
        auto elapsed_secs = get_time_elapsed(end - begin + ammortization_time);
        auto wait_time_in_secs = FRAME_TIME - elapsed_secs;
#if __linux__
        auto wait_time_in_usecs = wait_time_in_secs * 1e6;
        if (wait_time_in_usecs >= 1) {
            usleep(static_cast<__useconds_t>(wait_time_in_usecs));
#elif _WIN32
        auto wait_time_in_msecs = static_cast<int>(wait_time_in_secs * 1000);
        if (wait_time_in_msecs >= 1) {
            std::cout << "sleeping " << wait_time_in_msecs << " msec" << std::endl;
            Sleep(wait_time_in_msecs);
#endif
        }
        end = clock();
#if PERFORMANCE_TEST
        cnt++;
        if (cnt == 300) {
            exit(0);
        }
#endif
    });
    return 0;
}

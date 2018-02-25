#include <memory>
#include "glfw_window_manager.h"
#include "gl_holder.h"
#include <unistd.h>
#include <iostream>

static const double FPS = 60.0;
static const double FRAME_TIME = 1.0 / FPS;

int main() {
    auto window_manager = std::make_shared<GLFWWindowManager>("Salute", 1024, 768);
    auto gl_holder = std::make_shared<GLHolder>(window_manager);

    clock_t ammortization_time = 0;
    
    clock_t begin = 0, end = 0;
    
    window_manager->main_loop([&]() {
        ammortization_time = end - begin;
        begin = clock();
        gl_holder->paint();
//        usleep(200*1e3);
        end = clock();
        auto elapsed_secs = double(end - begin + ammortization_time) / CLOCKS_PER_SEC;
        const double paint_time = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << paint_time * 1000 << "ms" << std::endl;
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

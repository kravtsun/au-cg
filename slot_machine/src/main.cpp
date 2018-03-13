#include <memory>
#if __linux__
#include <unistd.h>
#elif _WIN32
#include <Windows.h>
#endif

#include "gl_holder.h"
#include "glut_window_manager.h"
#include "FPSRenderPass.h"

static const double FPS = 60.0;
static const double FRAME_TIME = 1.0 / FPS;

#if defined(__unix__)
#include <pthread.h>
static void* simpleFunc(void*) {
    return NULL;
}
static void forcePThreadLink() {
    pthread_t t1;
    pthread_create(&t1, NULL, &simpleFunc, NULL);
}
#endif

int main(int argc, char **argv) {
    auto window_manager = std::make_shared<GLUTWindowManager>("SlotMachine", 1024, 768, argc, argv);
    auto gl_holder = std::make_shared<GLHolder>(window_manager);

    time_point begin, end;
    auto main_loop_func = [&]() {
        begin = now_time();
        auto const ammortization_time = get_seconds(begin, end);
        gl_holder->paint();
        end = now_time();
        auto elapsed_secs = get_seconds(end, begin) + ammortization_time;
        auto wait_time_in_secs = FRAME_TIME - elapsed_secs;
#if __linux__
        auto wait_time_in_usecs = wait_time_in_secs * 1e6;
        if (wait_time_in_usecs >= 1) {
            usleep(static_cast<__useconds_t>(wait_time_in_usecs));
#elif _WIN32
        auto wait_time_in_msecs = static_cast<int>(wait_time_in_secs * 1000);
        if (wait_time_in_msecs >= 1) {
            Sleep(wait_time_in_msecs);
#endif
        }
        end = now_time();
    };
    
    auto mouse_action = [&](int button, int state, int x, int y) {
        gl_holder->click(x, y);
    };

    window_manager->set_mouse_action(mouse_action);
    window_manager->main_loop(main_loop_func);

    return 0;
}


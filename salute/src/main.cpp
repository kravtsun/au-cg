#include <memory>
#include "glfw_window_manager.h"
#include "gl_holder.h"
#include <unistd.h>




int main() {
    auto window_manager = std::make_shared<GLFWWindowManager>("Salute", 1024, 768);
    auto gl_holder = std::make_shared<GLHolder>(window_manager);
    
    window_manager->main_loop([&gl_holder]() {
        gl_holder->paint();
        usleep(3e5);
    });
    return 0;
}

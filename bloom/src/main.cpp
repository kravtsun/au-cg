#include <memory>
#include <AntTweakBar.h>
#include "glfw_window_manager.h"
#include "gl_holder.h"
#include "atb.h"

int main() {
    auto window_manager = std::make_shared<GLFWWindowManager>("Deferred", 1024, 768);
    auto gl_holder = std::make_shared<GLHolder>(window_manager);
    atb::init(window_manager, gl_holder);
    
    window_manager->main_loop(atb::draw);
    TwTerminate();
    
    return 0;
}

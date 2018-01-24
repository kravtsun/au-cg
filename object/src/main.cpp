#include <memory>
#include "glfw_window_manager.h"
#include "GLHolder.h"

int main() {
    std::shared_ptr<GLFWWindowManager> window_manager = std::make_shared<GLFWWindowManager>("Object", 1024, 768);
    
    GLHolder gl(window_manager);
    
    window_manager->main_loop([&]()
                              {
                                  gl.paint();
                              });

    return 0;
}

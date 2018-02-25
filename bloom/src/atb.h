#ifndef ATB_H
#define ATB_H

#include <memory>

class GLFWWindowManager;

class GLHolder;

namespace atb {
void init(std::shared_ptr<GLFWWindowManager> window_manager, std::shared_ptr<GLHolder> gl_holder);

void draw();
}
#endif // ATB_H

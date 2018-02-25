#ifndef DEFERRED_GL_LOADER_H
#define DEFERRED_GL_LOADER_H

#include <GL/glew.h>
#include <memory>

#include "glfw_window_manager.h"

class GLHolder {
public:
    bool isPaused = false;
    
    explicit GLHolder(std::shared_ptr<GLFWWindowManager> window_manager);
    
    virtual void paint();
    
    virtual ~GLHolder();
    
    std::shared_ptr<GLFWWindowManager> window_manager;
};


#endif //DEFERRED_GL_LOADER_H

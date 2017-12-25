#ifndef OBJECT_GL_H
#define OBJECT_GL_H

#include "glfw_window_manager.h"

struct GLHolder {
    explicit GLHolder(std::shared_ptr<GLFWWindowManager> window_manager);
    
    void paint();
    
    virtual ~GLHolder();
    
private:
    std::shared_ptr<GLFWWindowManager> window_manager;
};


#endif //OBJECT_GL_H

#ifndef DEFERRED_GL_LOADER_H
#define DEFERRED_GL_LOADER_H

#include <GL/glew.h>
#include <memory>

#include "glfw_window_manager.h"
#include "FramePass.h"
#include "PassthroughPass.h"
#include "FrameCombinerPass.h"

class GLHolder {
public:
    explicit GLHolder(std::shared_ptr<GLFWWindowManager> window_manager);
    
    int width() const {
        return window_manager->win_width();
    }
    
    int height() const {
        return window_manager->win_height();
    }
    
    virtual void paint();
    
    virtual ~GLHolder();

private:
    std::shared_ptr<GLFWWindowManager> window_manager;
    
    FramePass frame_pass;
    PassthroughPass passthrough_pass;
    FrameCombinerPass frame_combiner_pass;
};


#endif //DEFERRED_GL_LOADER_H

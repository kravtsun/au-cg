#ifndef DEFERRED_GL_LOADER_H
#define DEFERRED_GL_LOADER_H

#include <GL/glew.h>
#include <memory>

#include "FramePass.h"
#include "PassthroughPass.h"
#include "FrameCombinerPass.h"
#include "SkyPass.h"

struct GLFWWindowManager;

class GLHolder {
public:
    explicit GLHolder(std::shared_ptr<GLFWWindowManager> window_manager);
    
    int width() const;
    
    int height() const;
    
    virtual void paint();
    
    virtual ~GLHolder();

private:
    std::shared_ptr<GLFWWindowManager> window_manager;
    
    FramePass frame_pass;
    PassthroughPass passthrough_pass;
    FrameCombinerPass frame_combiner_pass;
    
    SkyPass sky_pass;
};


#endif //DEFERRED_GL_LOADER_H

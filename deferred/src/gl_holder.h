#ifndef DEFERRED_GL_LOADER_H
#define DEFERRED_GL_LOADER_H

#include <GL/glew.h>
#include <memory>

#include "glfw_window_manager.h"
#include "TexturePass.h"
#include "GeometryPass.h"
#include "LightPass.h"
#include "ThresholdPass.h"
#include "BlurPass.h"
#include "AdditivePass.h"

struct Light;

class GLHolder {
public:
    explicit GLHolder(std::shared_ptr<GLFWWindowManager> window_manager);
    
    virtual void paint();
    
    virtual ~GLHolder();
    
    static constexpr int max_lights_count = 10;
    unsigned int lights_count = max_lights_count;
    
    bool bloom_is_on = true;
    GLfloat bloom_threshold = 0.7;
    
    bool pause = false;
    
//private: // TODO fix privacy leak!
    std::vector<std::shared_ptr<Light>> lights;
    std::shared_ptr<GLFWWindowManager> window_manager;
    TexturePass texture_pass;
    GeometryPass geometry_pass;
    LightPass light_pass;
    ThresholdPass threshold_pass;
    BlurPass blur_pass;
    AdditivePass additive_pass;
};


#endif //DEFERRED_GL_LOADER_H

#ifndef DEFERRED_GL_LOADER_H
#define DEFERRED_GL_LOADER_H

#include <GL/glew.h>
#include <memory>

#include "glfw_window_manager.h"
#include "TexturePass.h"
#include "GeometryPass.h"
#include "LightPass.h"

struct Light;

class GLHolder {
public:
    explicit GLHolder(std::shared_ptr<GLFWWindowManager> window_manager);
    
    virtual void paint();
    
    virtual ~GLHolder();
    
    static constexpr int maxLightsCount = 10;
    unsigned int lightsCount = maxLightsCount;
    
//private: // TODO fix privacy leak!
    std::vector<std::shared_ptr<Light>> lights;
    std::shared_ptr<GLFWWindowManager> window_manager;
    TexturePass texturePass;
    GeometryPass geometryPass;
    LightPass lightPass;
};


#endif //DEFERRED_GL_LOADER_H

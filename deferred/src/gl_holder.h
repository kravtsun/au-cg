#ifndef DEFERRED_GL_LOADER_H
#define DEFERRED_GL_LOADER_H

#include <GL/glew.h>
#include <glfw_window_manager.h>
#include <memory>

class GLHolder {
public:
    explicit GLHolder(std::shared_ptr<GLFWWindowManager> window_manager);
    
    virtual void paint();
    
    virtual ~GLHolder();
    
    static constexpr int maxLightsCount = 10;
    int lightsCount = maxLightsCount;
    
    float screenGamma = 2.2;
    
    enum Mode {
        POSITION,
        NORMAL,
        DIFFUSE,
        AMBIENT,
        DEFERRED
    };
    Mode mode = DEFERRED;
private:
    std::shared_ptr<GLFWWindowManager> window_manager;
    
    void geometry_pass();
    
    void light_pass();
};


#endif //DEFERRED_GL_LOADER_H

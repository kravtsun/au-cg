#ifndef DEFERRED_GL_LOADER_H
#define DEFERRED_GL_LOADER_H

#include <memory>
#include <vector>
#include <GL/glew.h>

#include "SalutePass.h"
#include "PassthroughPass.h"
#include "SkyPass.h"

struct GLFWWindowManager;

class GLHolder {
public:
    explicit GLHolder(std::shared_ptr<GLFWWindowManager> window_manager);
    
    int width() const;
    
    int height() const;
    
    void add_salute(double x, double y, const glm::vec3 &color=glm::vec3(1, 1, 1));
    
    virtual void paint();
    
    virtual ~GLHolder();

private:
    std::shared_ptr<GLFWWindowManager> window_manager;
    
    std::vector<std::shared_ptr<SalutePass>> salutes;
    
    FrameCombinerPass salutes_combiner_pass;
    
    PassthroughPass passthrough_pass;
    
    SkyPass sky_pass;
};


#endif //DEFERRED_GL_LOADER_H

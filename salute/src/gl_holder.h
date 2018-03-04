#ifndef DEFERRED_GL_LOADER_H
#define DEFERRED_GL_LOADER_H

#include <memory>
#include <list>

#include "SalutePass.h"
#include "SaluteCombinerPass.h"
#include "PassthroughPass.h"
#include "SkyPass.h"

struct GLFWWindowManager;

struct GLHolder {
    explicit GLHolder(std::shared_ptr<GLFWWindowManager> window_manager);
    
    int width() const;
    
    int height() const;
    
    void add_salute(double x, double y);
    
    virtual void paint();
    
    virtual ~GLHolder();

private:
    std::shared_ptr<GLFWWindowManager> window_manager;
    
    std::list<std::shared_ptr<SalutePass>> salutes;
    
    SaluteCombinerPass salutes_combiner_pass;
    
    PassthroughPass passthrough_pass;
    
    SkyPass sky_pass;
};

#endif //DEFERRED_GL_LOADER_H

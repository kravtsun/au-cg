#ifndef SLOT_MACHINE_GL_HOLDER_H
#define SLOT_MACHINE_GL_HOLDER_H

#include <memory>
#include <GL/glew.h>

#include "PassthroughPass.h"
#include "CellPass.h"

struct GLFWWindowManager;

struct GLHolder {
    explicit GLHolder(std::shared_ptr<GLFWWindowManager> window_manager);
    
    int width() const;
    
    int height() const;
    
    virtual void paint();
    
    virtual ~GLHolder();

private:
    std::shared_ptr<GLFWWindowManager> window_manager;
    CellPass cell_pass;
    PassthroughPass passthrough_pass;
};

#endif //SLOT_MACHINE_GL_HOLDER_H

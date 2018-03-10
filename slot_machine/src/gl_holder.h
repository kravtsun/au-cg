#ifndef SLOT_MACHINE_GL_HOLDER_H
#define SLOT_MACHINE_GL_HOLDER_H

#include <memory>
#include <GL/glew.h>

#include "PassthroughPass.h"
#include "CellsPass.h"
#include "StartButtonPass.h"

struct WindowManager;

struct GLHolder {
    explicit GLHolder(std::shared_ptr<WindowManager> window_manager);
    
    int width() const;
    
    int height() const;
    
    virtual void paint();
    
    virtual ~GLHolder();

private:
    std::shared_ptr<WindowManager> window_manager;
    CellsPass cells_pass;
    StartButtonPass start_button_pass;
    PassthroughPass passthrough_pass;
};

#endif //SLOT_MACHINE_GL_HOLDER_H

#ifndef SLOT_MACHINE_GLUT_WINDOW_MANAGER_H
#define SLOT_MACHINE_GLUT_WINDOW_MANAGER_H

#include "window_manager.h"

struct GLUTWindowManager : WindowManager {
    GLUTWindowManager(const std::string &win_name, int win_width, int win_height, int argc, char **argv);
    
    void main_loop(std::function<void()> inside_action) const override;
    
    virtual ~GLUTWindowManager() = default;
};

#endif //SLOT_MACHINE_GLUT_WINDOW_MANAGER_H

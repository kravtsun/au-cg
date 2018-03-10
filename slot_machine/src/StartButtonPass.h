#ifndef SLOT_MACHINE_STARTBUTTONPASS_H
#define SLOT_MACHINE_STARTBUTTONPASS_H

#include <string>
#include <glm/glm.hpp>
#include "PassthroughPass.h"
#include "StartButton.h"
#include "color/ColorSupplier.h"
#include "color/ConstantColor.h"
#include "wrappers/AllWrappersFwd.h"

struct StartButtonPass: PassthroughPass {
    StartButtonPass(int width, int height,
                    const glm::ivec2 &position, const glm::ivec2 &size);
    
    void pass() override;
    
    void set_fgcolor(const ColorSupplierWrapper &new_fgcolor);
    
    void set_bgcolor(const ColorSupplierWrapper &new_bgcolor);
    
    ~StartButtonPass() final = default;

private:
    StartButton start_button;
    ColorSupplierWrapper bgcolor;
    ColorSupplierWrapper fgcolor;
    
    ProgramWrapper program;
    GLint pass_texture_id, fg_color_id, bg_color_id;
};

#endif //SLOT_MACHINE_STARTBUTTONPASS_H

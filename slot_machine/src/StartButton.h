#ifndef SLOT_MACHINE_STARTBUTTON_H
#define SLOT_MACHINE_STARTBUTTON_H

#include "Button.h"
#include "wrappers/AllWrappersFwd.h"

struct StartButton : Button {
    StartButton(const glm::ivec2 &position, const glm::ivec2 &size);
    
    TextureWrapper get_texture() const override {
        return texture;
    }

private:
    TextureWrapper texture;
};

#endif //SLOT_MACHINE_STARTBUTTON_H

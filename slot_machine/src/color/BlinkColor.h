#ifndef SLOT_MACHINE_BLINKER_H
#define SLOT_MACHINE_BLINKER_H

#include <glm/vec4.hpp>
#include "ColorSupplier.h"

struct BlinkColor final: ColorSupplier {
    explicit BlinkColor(size_t nblink_frames,
                        const glm::vec4 &front_color=glm::vec4(1),
                        const glm::vec4 &back_color=glm::vec4(0.5))
            : nblink_frames(nblink_frames)
            , front_color(front_color)
            , back_color(back_color)
    {}
    
    glm::vec4 get_color() override;

private:
    mutable size_t nframes = 0;
    size_t nblink_frames;
    
    glm::vec4 front_color;
    glm::vec4 back_color;
};


#endif //SLOT_MACHINE_BLINKER_H

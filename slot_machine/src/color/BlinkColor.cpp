#include <utility>
#include "BlinkColor.h"

glm::vec4 BlinkColor::get_color() {
    if (nframes % nblink_frames == 0) {
        std::swap(front_color, back_color);
    }
    ++nframes;
    return front_color;
}

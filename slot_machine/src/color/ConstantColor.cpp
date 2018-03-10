#include "ConstantColor.h"

ConstantColor::ConstantColor(const glm::vec4 &color)
        : color(color)
{}

glm::vec4 ConstantColor::get_color() {
    return color;
}

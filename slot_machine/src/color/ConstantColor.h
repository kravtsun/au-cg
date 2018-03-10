#ifndef SLOT_MACHINE_CONSTANTCOLOR_H
#define SLOT_MACHINE_CONSTANTCOLOR_H

#include "ColorSupplier.h"

struct ConstantColor final: ColorSupplier {
    explicit ConstantColor(const glm::vec4 &color);
    
    glm::vec4 get_color() override;

private:
    glm::vec4 color;
};

#endif //SLOT_MACHINE_CONSTANTCOLOR_H

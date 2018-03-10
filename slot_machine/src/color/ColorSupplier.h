#ifndef SLOT_MACHINE_COLORSUPPLIER_H
#define SLOT_MACHINE_COLORSUPPLIER_H

#include <memory>
#include <glm/vec4.hpp>

struct ColorSupplier {
    virtual glm::vec4 get_color() = 0;
    
    virtual ~ColorSupplier() = default;
};

typedef std::shared_ptr<ColorSupplier> ColorSupplierWrapper;

#endif //SLOT_MACHINE_COLORSUPPLIER_H

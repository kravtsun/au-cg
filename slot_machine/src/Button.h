#ifndef SLOT_MACHINE_BUTTON_H
#define SLOT_MACHINE_BUTTON_H

#include <glm/glm.hpp>
#include "wrappers/AllWrappersFwd.h"

struct Button {
    Button(const glm::ivec2 &position, const glm::ivec2 &size);
    
    virtual void prepare();
    
    virtual TextureWrapper get_texture() const = 0;
    
    virtual ~Button() = default;
    
    void set_position(const glm::ivec2 &new_position) {
        position = new_position;
    }
    
    glm::ivec2 get_position() const {
        return position;
    }
    
    void set_size(const glm::ivec2 &new_size) {
        size = new_size;
    }
    
    glm::ivec2 get_size() const {
        return size;
    }
    
private:
    glm::ivec2 position, size;
};


#endif //SLOT_MACHINE_BUTTON_H

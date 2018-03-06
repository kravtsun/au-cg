#ifndef SLOT_MACHINE_CELLPASS_H
#define SLOT_MACHINE_CELLPASS_H

#include <map>
#include <vector>
#include <string>

#include <glm/vec2.hpp>

#include "AbstractPass.h"
#include "PassthroughPass.h"

struct CellHolder {
    static constexpr size_t CELL_COUNT = 3;
    static constexpr size_t SYMBOL_TEXTURE_WIDTH = 300;
    static constexpr size_t SYMBOL_TEXTURE_HEIGHT = 300;
    
    // speed, display position, lines, symbol_width, symbol_height,
    CellHolder(const glm::ivec2 &roi_pos,
               const glm::ivec2 &roi_size,
               const std::vector<std::string> &lines,
               const std::map<char, uchar *> &textures_data);

    void pass(const ProgramWrapper &program);
    
    void set_yspeed(GLfloat new_yspeed) {
        yspeed = new_yspeed;
    }
    
private:
    TextureWrapper texture;
    
    const std::vector<std::string> lines;
    const GLfloat ysize;
    GLfloat ymin = 0.f;
    GLfloat yspeed = 0.f;
    
    glm::ivec2 roi_pos, roi_size;
};


#endif //SLOT_MACHINE_CELLPASS_H

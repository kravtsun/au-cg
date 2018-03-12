#ifndef SLOT_MACHINE_CELLPASS_H
#define SLOT_MACHINE_CELLPASS_H

#include <map>
#include <vector>
#include <string>

#include <glm/vec2.hpp>
#include "AbstractPass.h"
#include "Button.h"

struct CellHolder: Button {
    static constexpr size_t CELL_COUNT = 3;
    static constexpr size_t SYMBOL_TEXTURE_WIDTH = 300;
    static constexpr size_t SYMBOL_TEXTURE_HEIGHT = 300;
    
    CellHolder(const glm::ivec2 &roi_pos,
               const glm::ivec2 &roi_size,
               const std::vector<std::string> &lines,
               const std::map<char, uchar *> &textures_data);

    void step() override;
    
    void set_yspeed(double new_yspeed) {
        yspeed = new_yspeed;
    }
    
    double get_ymin() const {
        return ymin;
    }
    
    void set_ymin(double new_ymin) {
        ymin = new_ymin;
    }
    
    double get_ymax() const {
        return ymin + ysize;
    }
    
    TextureWrapper get_texture() const override {
        return texture;
    }
    
private:
    TextureWrapper texture;
    const std::vector<std::string> lines;
    const double ysize;
    double ymin = 0;
    double yspeed = 0;
};


#endif //SLOT_MACHINE_CELLPASS_H

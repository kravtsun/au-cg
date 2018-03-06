#ifndef SLOT_MACHINE_CELLPASS_H
#define SLOT_MACHINE_CELLPASS_H

#include <vector>
#include <string>

#include <glm/vec2.hpp>

#include "AbstractPass.h"
#include "PassthroughPass.h"

struct CellPass {
    // speed, display position, lines, symbol_width, symbol_height,
    CellPass(const glm::ivec2 &roi_pos, const glm::ivec2 &roi_size, const std::vector<std::string> &lines);

    static TextureWrapper output_texture() {
        return color_texture;
    }
    
    void set_yspeed(GLfloat new_yspeed) {
        yspeed = new_yspeed;
    }
    
    static void prepare_passthrough(int win_width, int win_height);
    
    static void prepare_pass();
    
    void pass();
    
private:
    TextureWrapper texture;
    
    static ProgramWrapper program;
    static GLint pass_texture_id;
    static GLint ymin_id, ymax_id;
    
    const std::vector<std::string> lines;
    const GLfloat ysize;
    GLfloat ymin = 0.f;
    GLfloat yspeed = 0.f;
    
    glm::ivec2 roi_pos, roi_size;
    
    static std::shared_ptr<PassthroughPass> passthrough_pass;
    static FramebufferWrapper fbo;
    static TextureWrapper color_texture;
};


#endif //SLOT_MACHINE_CELLPASS_H

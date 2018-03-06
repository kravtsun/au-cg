#ifndef SLOT_MACHINE_CELLPASS_H
#define SLOT_MACHINE_CELLPASS_H

#include <vector>
#include <string>

#include "AbstractPass.h"
#include "PassthroughPass.h"

struct CellPass : PassthroughPass {
    // speed, display position, lines, symbol_width, symbol_height,
    CellPass(int win_width, int win_height);

    TextureWrapper output_texture() const override {
        return color_texture;
    }
    
    void pass() override;
    
private:
    std::vector<std::string> lines;
//    std::vector<std::vector<TextureWrapper>> textures;
    TextureWrapper texture;
    
    ProgramWrapper program;
    GLint pass_texture_id;
    
    GLint ymin_id, ymax_id;
    GLfloat ymin = 0.f, ysize = 0.333333f;
    
    FramebufferWrapper fbo;
    TextureWrapper color_texture;
};


#endif //SLOT_MACHINE_CELLPASS_H

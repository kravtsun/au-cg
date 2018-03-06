#ifndef SLOT_MACHINE_CELLSPASS_H
#define SLOT_MACHINE_CELLSPASS_H

#include "PassthroughPass.h"
#include "CellHolder.h"

struct CellsPass : PassthroughPass {
    CellsPass(int width, int height);
    
    void pass() override;
    
    TextureWrapper output_texture() const override {
        return color_texture;
    }
    
    ~CellsPass() final = default;

private:
    ProgramWrapper program;
    std::vector<CellHolder> cell_holders;
    
    FramebufferWrapper fbo;
    TextureWrapper color_texture;
};


#endif //SLOT_MACHINE_CELLSPASS_H

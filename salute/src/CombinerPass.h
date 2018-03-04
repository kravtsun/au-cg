#ifndef SALUTE_COMBINERPASS_H
#define SALUTE_COMBINERPASS_H

#include "PassthroughPass.h"

struct CombinerPass : public PassthroughPass {
    CombinerPass(int width, int height);

    TextureWrapper output_texture() const override {
        return front_texture;
    }
    
    void pass() override;
    
    void reset();
    
    virtual const ProgramWrapper &get_program() const = 0;
    
    virtual ~CombinerPass();
    
private:
    FramebufferWrapper fbo;
    TextureWrapper front_texture, back_texture;
};


#endif //SALUTE_COMBINERPASS_H

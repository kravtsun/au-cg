#ifndef SALUTE_COMBINERPASS_H
#define SALUTE_COMBINERPASS_H

#include "PassthroughPass.h"

struct CombinerPass : public PassthroughPass {
    CombinerPass(int width, int height);

    TextureWrapper output_texture() const override;
    
    void reset();
    
    virtual const ProgramWrapper &get_program() const = 0;
    
    virtual ~CombinerPass();
    
protected:
    FramebufferWrapper fbo;
    TextureWrapper front_texture, back_texture;
};


#endif //SALUTE_COMBINERPASS_H

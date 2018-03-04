#ifndef SALUTE_FRAMECOMBINERPASS_H
#define SALUTE_FRAMECOMBINERPASS_H

#include "PassthroughPass.h"

struct FrameCombinerPass : public PassthroughPass {
    FrameCombinerPass(int width, int height, const std::string &fragment_path="combine.fsh");

    TextureWrapper output_texture() const override {
        return front_texture;
    }
    
    void pass() override;
    
    void reset();
    
    ~FrameCombinerPass() final;
    
private:
    ProgramWrapper program;
    GLint input_texture_id, accumulator_texture_id;
    
    FramebufferWrapper fbo;
    TextureWrapper front_texture, back_texture;
};


#endif //SALUTE_FRAMECOMBINERPASS_H

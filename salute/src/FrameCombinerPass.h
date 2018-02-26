#ifndef SALUTE_FRAMECOMBINERPASS_H
#define SALUTE_FRAMECOMBINERPASS_H

#include "PassthroughPass.h"

struct FrameCombinerPass : public PassthroughPass {
    FrameCombinerPass(int width, int height);

    GLuint output_texture() const override {
        return front_texture;
    }
    
    void pass() override;
    
    void reset();
    
    ~FrameCombinerPass() final;
    
private:
    GLuint program_id;
    GLint input_texture_id, accumulator_texture_id;
    
    GLuint fbo;
    GLuint front_texture, back_texture;
};


#endif //SALUTE_FRAMECOMBINERPASS_H

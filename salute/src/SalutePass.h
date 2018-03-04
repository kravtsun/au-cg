#ifndef SALUTE_SALUTEPASS_H
#define SALUTE_SALUTEPASS_H

#include <glm/vec3.hpp>
#include "AbstractPass.h"
#include "FramePass.h"
#include "FrameCombinerPass.h"

struct SalutePass : public AbstractPass {
    SalutePass(int width, int height, glm::vec3 position, const glm::vec3 &color);
    
    void pass() override;
    
    TextureWrapper output_texture() const override;
    
    void reset() {
        frame_pass.reset();
        frame_combiner_pass.reset();
    }
    
    ~SalutePass() final;

private:
    FramePass frame_pass;
    FrameCombinerPass frame_combiner_pass;
};

#endif //SALUTE_SALUTEPASS_H

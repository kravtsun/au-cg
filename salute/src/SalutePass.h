#ifndef SALUTE_SALUTEPASS_H
#define SALUTE_SALUTEPASS_H

#include <glm/vec3.hpp>
#include "AbstractPass.h"
#include "FramePass.h"
#include "FrameCombinerPass.h"

struct SalutePass : AbstractPass {
    SalutePass(int width, int height,
               const glm::vec3 &position, const glm::vec3 &color,
               int nparticles=100,
               float speed_magnitude=30);
    
    void pass() override;
    
    TextureWrapper output_texture() const override;
    
    void reset();
    
    bool is_alive() const;
    
    ~SalutePass() final;

private:
    FramePass frame_pass;
    FrameCombinerPass frame_combiner_pass;
};

#endif //SALUTE_SALUTEPASS_H

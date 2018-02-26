#ifndef SALUTE_FRAMEPASS_H
#define SALUTE_FRAMEPASS_H

#include "AbstractPass.h"

struct FramePass : public AbstractPass {
    // TODO explosion parameters:
    // - nparticles (convert to vector?).
    FramePass(int width, int height);

    void pass() override;

    ~FramePass() final;
    
    GLuint output_texture() const override {
        return color_texture;
    }
    
    void reset();
    
private:
    GLuint program_id;
    GLint camera_up_id, camera_right_id;
    GLint vp_id;
    
    GLint particle_start_id, particle_color_id, particle_size_id, time_after_explosion_id;
    
    GLuint vao, square_buffer, speed_buffer;
    
    GLuint fbo;
    GLuint color_texture, depth_texture;
    GLuint color_buffer;
    
    static constexpr float FPS = 60.f;
    static constexpr float time_delta = 1.f / FPS;
    float time_after_explosion = 0.f;
};


#endif //SALUTE_FRAMEPASS_H

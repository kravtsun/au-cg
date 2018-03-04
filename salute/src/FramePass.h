#ifndef SALUTE_FRAMEPASS_H
#define SALUTE_FRAMEPASS_H

#include "AbstractPass.h"
#include <glm/vec3.hpp>
#include <cstdio>

struct FramePass : public AbstractPass {
    // TODO explosion parameters:
    // - nparticles (convert to vector?).
    FramePass(int width, int height,
              const glm::vec3 &particle_start=glm::vec3(0, 0, 0),
              const glm::vec3 &particle_color=glm::vec3(1, 1, 1));

    void pass() override;

    ~FramePass() final;
    
    TextureWrapper output_texture() const override {
        return color_texture;
    }
    
    void reset();
    
    static constexpr float limit = 100.f;
    static constexpr float FPS = 60.f;
    
private:
    glm::vec3 particle_start, particle_color;
    
    ProgramWrapper program;
    GLint camera_up_id, camera_right_id;
    GLint vp_id;
    
    GLint particle_start_id, particle_color_id, particle_size_id,
            time_after_explosion_id, fade_multiplier_id, seconds_to_decelerate_id;
    
    GLuint vao, square_buffer, speed_buffer;

    FramebufferWrapper fbo;
    TextureWrapper color_texture, depth_texture;
    GLuint color_buffer;
    
    static constexpr float time_delta = 1.f / FPS;
    float time_after_explosion = 0.f;
    
    const float seconds_to_decelerate = 3.5;
    const float fade_start_time_normalized = 0.2;
    const float fade_speed = 20;
    
    float get_fade_multiplier() const;
    
    float is_alive() const {
        return get_fade_multiplier() > 0.01;
    }
};


#endif //SALUTE_FRAMEPASS_H

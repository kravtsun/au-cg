#include "SalutePass.h"

SalutePass::SalutePass(int width, int height,
                       const glm::vec3 &position, const glm::vec3 &color,
                       int nparticles,
                       float speed_magnitude)
        : AbstractPass(width, height)
        , frame_pass(width, height, position, color, nparticles, speed_magnitude)
        , frame_combiner_pass(width, height)
{}

SalutePass::~SalutePass() = default;


void SalutePass::pass() {
    frame_pass.pass();
    frame_combiner_pass.set_input_texture(frame_pass.output_texture());
    frame_combiner_pass.pass();
}

TextureWrapper SalutePass::output_texture() const {
    return frame_combiner_pass.output_texture();
}

void SalutePass::reset() {
    frame_pass.reset();
    frame_combiner_pass.reset();
}

bool SalutePass::is_alive() const {
    return frame_pass.get_fade_multiplier() > 0.01;
}


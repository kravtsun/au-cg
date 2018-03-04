#include "SalutePass.h"

SalutePass::SalutePass(int width, int height, glm::vec3 position, const glm::vec3 &color)
        : AbstractPass(width, height)
        , frame_pass(width, height, position, color)
        , frame_combiner_pass(width, height)
{
}

void SalutePass::pass() {
    frame_pass.pass();
    frame_combiner_pass.set_input_texture(frame_pass.output_texture());
    frame_combiner_pass.pass();
}

TextureWrapper SalutePass::output_texture() const {
    return frame_combiner_pass.output_texture();
}

SalutePass::~SalutePass() = default;

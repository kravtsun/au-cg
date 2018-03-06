#if _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <string>

#include "SkyPass.h"
#include "wrappers/AllWrappers.h"

SkyPass::SkyPass(int width, int height, const std::string &filename)
        : PassthroughPass(width, height, false)
        , program_id(new Program("pass_texture.vsh", "sky.fsh"))
{
    bias_id = glGetUniformLocation(*program_id, "bias");
    input_texture_id = glGetUniformLocation(*program_id, "input_texture");
    input_texture = load_bmp_texture(filename);
    init_framebuffer_with_output_texture(fbo, color_texture);
}

SkyPass::~SkyPass() = default;

void SkyPass::pass() {
    program_id->use();
    input_texture->bind_as_input(input_texture_id);
    glUniform2f(bias_id, step.x, step.y);
    step.x += 0.25f / get_width();
    
    if (step.x > 1) {
        step.x -= 1;
    }
    
    fbo->bind();
    input_texture->bind_as_input(input_texture_id, 0);
    glViewport(0, 0, get_width(), get_height());
    glClear(GL_COLOR_BUFFER_BIT);
    draw_quad();
}

TextureWrapper SkyPass::output_texture() const {
    return color_texture;
}

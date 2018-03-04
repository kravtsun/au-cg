#include <cassert>
#include <algorithm>
#include <stdexcept>
#include "FrameCombinerPass.h"
#include "wrappers/AllWrappers.h"

FrameCombinerPass::FrameCombinerPass(int width, int height, const std::string &fragment_path)
        : PassthroughPass(width, height, false)
        , program(new Program("pass_texture.vsh", fragment_path.c_str()))
{
    input_texture_id = glGetUniformLocation(*program, "input_texture");
    accumulator_texture_id = glGetUniformLocation(*program, "output_texture");
    
    init_framebuffer_with_output_texture(fbo, front_texture);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    init_and_bind_empty_texture(back_texture, get_width(), get_height());
//    std::swap(front_texture, back_texture);
}

void FrameCombinerPass::pass() {
    check_input_texture_set("FrameCombinerPass");
    
    program->use();
    glActiveTexture(GL_TEXTURE0);
    front_texture->bind();
    glUniform1i(accumulator_texture_id, 0);
    
    glActiveTexture(GL_TEXTURE1);
    get_input_texture()->bind();
    glUniform1i(input_texture_id, 1);
    
    fbo->bind();
    glViewport(0, 0, get_width(), get_height());
    back_texture->reset();
    glDisable(GL_BLEND); // done in shader.

    draw_quad();
    std::swap(front_texture, back_texture);
}

FrameCombinerPass::~FrameCombinerPass() = default;

void FrameCombinerPass::reset() {
    fbo->bind();
    front_texture->reset();
    back_texture->reset();
}

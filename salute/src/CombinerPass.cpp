#include <cassert>
#include <algorithm>
#include <stdexcept>
#include "CombinerPass.h"
#include "wrappers/AllWrappers.h"

CombinerPass::CombinerPass(int width, int height)
        : PassthroughPass(width, height, false)
{
    init_framebuffer_with_output_texture(fbo, front_texture);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    init_and_bind_empty_texture(back_texture, get_width(), get_height());
//    std::swap(front_texture, back_texture);
}

void CombinerPass::pass() {
    check_input_texture_set("CombinerPass");
    
    get_program()->use();
    glActiveTexture(GL_TEXTURE0);
    front_texture->bind();
    
    const GLint accumulator_texture_id = glGetUniformLocation(*get_program(), "output_texture");
    glUniform1i(accumulator_texture_id, 0);
    
    glActiveTexture(GL_TEXTURE1);
    get_input_texture()->bind();
    
    const GLint input_texture_id = glGetUniformLocation(*get_program(), "input_texture");
    glUniform1i(input_texture_id, 1);
    
    fbo->bind();
    back_texture->reset();
    glDisable(GL_BLEND); // done in shader.

    draw_quad();
    std::swap(front_texture, back_texture);
}

CombinerPass::~CombinerPass() = default;

void CombinerPass::reset() {
    fbo->bind();
    front_texture->reset();
    back_texture->reset();
}

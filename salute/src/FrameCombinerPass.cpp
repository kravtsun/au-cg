#include <cassert>
#include <algorithm>
#include <stdexcept>
#include "FrameCombinerPass.h"
#include "shader.h"

FrameCombinerPass::FrameCombinerPass(const int width, const int height)
        : PassthroughPass(width, height, false)
{
    program_id = load_shaders("pass_texture.vsh", "combine.fsh");
    input_texture_id = glGetUniformLocation(program_id, "input_texture");
    accumulator_texture_id = glGetUniformLocation(program_id, "output_texture");
    
    init_framebuffer_with_output_texture(fbo, front_texture);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    init_and_bind_output_texture(back_texture);
//    std::swap(front_texture, back_texture);
}

void FrameCombinerPass::pass() {
    check_input_texture_set("FrameCombinerPass");
    
    glUseProgram(program_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, front_texture);
    glUniform1i(accumulator_texture_id, 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, get_input_texture());
    glUniform1i(input_texture_id, 1);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, back_texture, 0);
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(fbo_status == GL_FRAMEBUFFER_COMPLETE);
    
    glViewport(0, 0, get_width(), get_height());
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND); // done in shader.

    draw_quad();
    std::swap(front_texture, back_texture);
}

FrameCombinerPass::~FrameCombinerPass() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &front_texture);
    glDeleteTextures(1, &back_texture);
    glDeleteProgram(program_id);
}

void FrameCombinerPass::reset() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    auto reset_texture = [&](const GLuint &texture) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        assert(fbo_status == GL_FRAMEBUFFER_COMPLETE);
        glClear(GL_COLOR_BUFFER_BIT);
    };
    reset_texture(front_texture);
    reset_texture(back_texture);
}

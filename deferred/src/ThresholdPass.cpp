#include <cassert>
#include <shader.h>

#include "ThresholdPass.h"

void ThresholdPass::init_mask_render_buffer() {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    glGenTextures(1, &mask_texture);
    init_texture(getWidth(), getHeight(), mask_texture, GL_COLOR_ATTACHMENT0);
    
    const GLenum DrawBuffers[] = {
            GL_COLOR_ATTACHMENT0
    };
    const GLsizei drawBuffersCount = sizeof(DrawBuffers) / sizeof(DrawBuffers[0]);
    static_assert(drawBuffersCount == 1, "drawBuffersCount == 1");
    glDrawBuffers(drawBuffersCount, DrawBuffers);
    
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(fbo_status == GL_FRAMEBUFFER_COMPLETE);
}

ThresholdPass::ThresholdPass(int width, int height)
        : Pass(width, height) {
    init_mask_render_buffer();
    program_id = load_shaders("PassthroughTexture.vsh", "threshold.fsh");
    threshold_id = glGetUniformLocation(program_id, "threshold");
    color_texture_id = glGetUniformLocation(program_id, "colorTexture");
}

void ThresholdPass::pass(GLuint colorTexture, GLfloat threshold) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, getWidth(), getHeight());
    
    glUseProgram(program_id);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glUniform1i(color_texture_id, 0);
    
    glUniform1f(threshold_id, threshold);
    drawTexture();
}

ThresholdPass::~ThresholdPass() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &mask_texture);
}

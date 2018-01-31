#include <cassert>
#include <glm/glm.hpp>
#include "BlurPass.h"
#include "shader.h"

void BlurPass::init_blur(GLuint &fbo, GLuint &output_texture)
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    glGenTextures(1, &output_texture);
    init_texture(getWidth(), getHeight(), output_texture, GL_COLOR_ATTACHMENT0);
    
    const GLenum DrawBuffers[] = {
            GL_COLOR_ATTACHMENT0
    };
    const GLsizei drawBuffersCount = sizeof(DrawBuffers) / sizeof(DrawBuffers[0]);
    static_assert(drawBuffersCount == 1, "drawBuffersCount == 1");
    glDrawBuffers(drawBuffersCount, DrawBuffers);
    
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(fbo_status == GL_FRAMEBUFFER_COMPLETE);
}

BlurPass::BlurPass(int width, int height)
        : Pass(width, height)
{
    init_blur(fboh, outputh);
    init_blur(fbov, outputv);
    program_id = load_shaders("PassthroughTexture.vsh", "blur.fsh");
//    kernel_id = glGetUniformLocation(program_id, "kernel");
    input_texture_id = glGetUniformLocation(program_id, "inputTexture");
    step_id = glGetUniformLocation(program_id, "step");
}

void BlurPass::pass(GLuint inputTexture) {
    glBindFramebuffer(GL_FRAMEBUFFER, fboh);
    glViewport(0, 0, getWidth(), getHeight());
    
    glUseProgram(program_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glUniform1i(input_texture_id, 0);
    
    glUniform2f(step_id, static_cast<GLfloat>(1.0 / getWidth()), 0.0);
    drawTexture();
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbov);
    glViewport(0, 0, getWidth(), getHeight());
    
    glBindTexture(GL_TEXTURE_2D, outputh);
    
    glUniform2f(step_id, 0.0, static_cast<GLfloat>(1.0 / getHeight()));
    drawTexture();
}

BlurPass::~BlurPass() {
    glDeleteFramebuffers(1, &fboh);
    glDeleteTextures(1, &outputh);
    glDeleteFramebuffers(1, &fbov);
    glDeleteTextures(1, &outputv);
}

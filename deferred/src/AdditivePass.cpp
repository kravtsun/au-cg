#include <cassert>
#include "AdditivePass.h"
#include "shader.h"

void AdditivePass::init_render_buffer() {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    init_output_texture(sum_texture);
}

AdditivePass::AdditivePass(int width, int height)
        : Pass(width, height)
{
    init_render_buffer();
    program_id = load_shaders("PassthroughTexture.vsh", "additive.fsh");
    first_texture_id = glGetUniformLocation(program_id, "firstTexture");
    second_texture_id = glGetUniformLocation(program_id, "secondTexture");
    second_multiplier_id = glGetUniformLocation(program_id, "secondMultiplier");
}

void AdditivePass::pass(GLuint firstTexture, GLuint secondTexture, GLfloat secondMultiplier) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, getWidth(), getHeight());
    glUseProgram(program_id);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, firstTexture);
    glUniform1i(first_texture_id, 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, secondTexture);
    glUniform1i(second_texture_id, 1);
    
    glUniform1f(second_multiplier_id, secondMultiplier);
    
    drawTexture();
}

GLuint AdditivePass::outputTexture() const {
    return sum_texture;
}

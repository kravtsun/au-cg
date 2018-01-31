#include "TexturePass.h"
#include "shader.h"

TexturePass::TexturePass(int width, int height) : Pass(width, height) {
    program_id = load_shaders("PassthroughTexture.vsh", "debug.fsh");
    texture_id = glGetUniformLocation(program_id, "renderedTexture");
}

void TexturePass::pass(GLuint texture) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(program_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(texture_id, 0);
    drawTexture();
}

TexturePass::~TexturePass() {
    glDeleteProgram(program_id);
}

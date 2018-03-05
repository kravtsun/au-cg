#include "SaluteCombinerPass.h"
#include "wrappers/AllWrappers.h"

ProgramWrapper SaluteCombinerPass::program;

SaluteCombinerPass::SaluteCombinerPass(int width, int height)
        : CombinerPass(width, height)
{
    if (program == nullptr) {
        program.reset(new Program("pass_texture.vsh", "pass_texture.fsh"));
    }
}

const ProgramWrapper &SaluteCombinerPass::get_program() const {
    return program;
}

void SaluteCombinerPass::pass() {
    check_input_texture_set("SaluteCombinerPass");
    
    get_program()->use();
    glActiveTexture(GL_TEXTURE0);
    get_input_texture()->bind();
    const GLint input_texture_id = glGetUniformLocation(*get_program(), "input_texture");
    glUniform1i(input_texture_id, 0);
    
    fbo->bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    
    draw_quad();
    glDisable(GL_BLEND);
}

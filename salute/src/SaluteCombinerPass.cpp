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
    const GLint input_texture_id = glGetUniformLocation(*get_program(), "input_texture");
    get_input_texture()->bind_as_input(input_texture_id, 0);
    
    fbo->bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    
    draw_quad();
    glDisable(GL_BLEND);
}

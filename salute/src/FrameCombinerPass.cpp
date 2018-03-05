#include "FrameCombinerPass.h"
#include "wrappers/AllWrappers.h"

ProgramWrapper FrameCombinerPass::program;

FrameCombinerPass::FrameCombinerPass(int width, int height)
        : CombinerPass(width, height)
{
    if (program == nullptr) {
        program.reset(new Program("pass_texture.vsh", "combine.fsh"));
    }
}

const ProgramWrapper &FrameCombinerPass::get_program() const {
    return program;
}

void FrameCombinerPass::pass() {
    check_input_texture_set("FrameCombinerPass");
    
    get_program()->use();
    const GLint accumulator_texture_id = glGetUniformLocation(*get_program(), "output_texture");
    front_texture->bind_as_input(accumulator_texture_id, 0);
    const GLint input_texture_id = glGetUniformLocation(*get_program(), "input_texture");
    get_input_texture()->bind_as_input(input_texture_id, 1);
    
    fbo->bind();
    back_texture->reset();
    glDisable(GL_BLEND); // done in shader.
    
    draw_quad();
    std::swap(front_texture, back_texture);
}

#include "SaluteCombinerPass.h"
#include "wrappers/AllWrappers.h"

ProgramWrapper SaluteCombinerPass::program;

SaluteCombinerPass::SaluteCombinerPass(int width, int height)
        : CombinerPass(width, height)
{
    if (program == nullptr) {
        program.reset(new Program("pass_texture.vsh", "combine_simple.fsh"));
    }
}

const ProgramWrapper &SaluteCombinerPass::get_program() const {
    return program;
}

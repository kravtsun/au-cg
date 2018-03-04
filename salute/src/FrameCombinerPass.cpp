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

#ifndef SALUTE_FRAMECOMBINERPASS_H
#define SALUTE_FRAMECOMBINERPASS_H

#include "CombinerPass.h"

struct FrameCombinerPass : CombinerPass {
    FrameCombinerPass(int width, int height);
    
    const ProgramWrapper &get_program() const override;
    
    void pass() override;

private:
    static ProgramWrapper program;
};


#endif //SALUTE_FRAMECOMBINERPASS_H

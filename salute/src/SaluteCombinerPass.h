#ifndef SALUTE_SALUTECOMBINERPASS_H
#define SALUTE_SALUTECOMBINERPASS_H

#include "CombinerPass.h"

struct SaluteCombinerPass : CombinerPass {
    SaluteCombinerPass(int width, int height);
    
    const ProgramWrapper &get_program() const override;
    
    void pass() override;

private:
    static ProgramWrapper program;
};

#endif //SALUTE_SALUTECOMBINERPASS_H

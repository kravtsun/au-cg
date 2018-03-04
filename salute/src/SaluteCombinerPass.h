#ifndef SALUTE_SALUTECOMBINERPASS_H
#define SALUTE_SALUTECOMBINERPASS_H

#include "CombinerPass.h"

struct SaluteCombinerPass : public CombinerPass {
    SaluteCombinerPass(int width, int height);
    
    const ProgramWrapper &get_program() const override;

private:
    static ProgramWrapper program;
};

#endif //SALUTE_SALUTECOMBINERPASS_H

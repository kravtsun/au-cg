#ifndef SLOT_MACHINE_FPSRENDERPASS_H
#define SLOT_MACHINE_FPSRENDERPASS_H

#include <ctime>
#include <GL/glew.h>
#include "AbstractPass.h"

struct FPSRenderPass : AbstractPass {
    FPSRenderPass(int width, int height);

    void pass() override;

    TextureWrapper output_texture() const override;

private:
    clock_t last_time = 0;
};

#endif // SLOT_MACHINE_FPSRENDERPASS_H
#include "CombinerPass.h"
#include "wrappers/AllWrappers.h"

CombinerPass::CombinerPass(int width, int height)
        : PassthroughPass(width, height, false)
{
    init_framebuffer_with_output_texture(fbo, front_texture);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    init_and_bind_empty_texture(back_texture, get_width(), get_height());
//    std::swap(front_texture, back_texture);
}

CombinerPass::~CombinerPass() = default;

TextureWrapper CombinerPass::output_texture() const {
    return front_texture;
}

void CombinerPass::reset() {
    fbo->bind();
    back_texture->reset();
    front_texture->reset();
}

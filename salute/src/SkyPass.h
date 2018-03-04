#ifndef SALUTE_SKYPASS_H
#define SALUTE_SKYPASS_H

#include <memory>
#include <glm/vec2.hpp>
#include "PassthroughPass.h"

struct SkyPass : public PassthroughPass {
    SkyPass(int width, int height, const std::string &filename);
    
    void pass() override;
    
    TextureWrapper output_texture() const override;
    
    ~SkyPass() final;
    
private:
    TextureWrapper input_texture, color_texture;
    ProgramWrapper program_id;
    GLint bias_id, input_texture_id;
    
    FramebufferWrapper fbo;
    
    glm::vec2 step;

private:
    static TextureWrapper load_bmp_texture(const std::string &filename);
};


#endif //SALUTE_SKYPASS_H

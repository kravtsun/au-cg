#ifndef SALUTE_SKYPASS_H
#define SALUTE_SKYPASS_H

#include <glm/vec2.hpp>
#include "PassthroughPass.h"

struct SkyPass : public PassthroughPass {
    SkyPass(int width, int height, const std::string &filename);
    
    void pass() override;
    
    GLuint output_texture() const override {
        return color_texture;
    }
    
    ~SkyPass() final;

private:
    GLuint input_texture, color_texture;
    GLuint program_id;
    GLint bias_id, input_texture_id;
    
    GLuint fbo;
    
    glm::vec2 step;

private:
    static GLuint load_bmp_texture(const std::string &filename);
};


#endif //SALUTE_SKYPASS_H

#ifndef DEFERRED_ADDITIVEPASS_H
#define DEFERRED_ADDITIVEPASS_H

#include "PassCommon.h"

struct AdditivePass : public Pass {
    GLuint fbo, sum_texture;
    
    GLuint program_id;
    GLint first_texture_id, second_texture_id;
    GLint second_multiplier_id;

public:
    AdditivePass(int width, int height);
    
    void pass(GLuint firstTexture, GLuint secondTexture, GLfloat secondMultiplier=1.0f);
    
    GLuint outputTexture() const override;

private:
    void init_render_buffer();
};


#endif //DEFERRED_ADDITIVEPASS_H

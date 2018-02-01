#ifndef DEFERRED_ADDITIVEPASS_H
#define DEFERRED_ADDITIVEPASS_H

#include "PassCommon.h"

struct AdditivePass : public Pass {
    GLuint fbo, sum_texture;
    
    GLuint program_id;
    GLint first_texture_id, second_texture_id;

public:
    AdditivePass(int width, int height);
    
    void pass(GLuint firstTexture, GLuint secondTexture);
    
    GLuint outputTexture() const override;

private:
    void init_render_buffer();
};


#endif //DEFERRED_ADDITIVEPASS_H

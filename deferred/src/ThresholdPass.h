#ifndef DEFERRED_THRESHOLDPASS_H
#define DEFERRED_THRESHOLDPASS_H

#include "PassCommon.h"

struct ThresholdPass : public Pass {
    GLuint fbo;
    GLuint mask_texture;
    
    GLuint program_id;
    GLint threshold_id, color_texture_id;

public:
    ThresholdPass(int width, int height);

    void pass(GLuint colorTexture, GLfloat threshold);
    
    virtual ~ThresholdPass();
    
private:
    void init_mask_render_buffer();
    
};


#endif //DEFERRED_THRESHOLDPASS_H

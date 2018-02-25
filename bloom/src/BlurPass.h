#ifndef DEFERRED_BLURPASS_H
#define DEFERRED_BLURPASS_H

#include "PassCommon.h"

struct BlurPass : public Pass {
    GLuint fboh, fbov;
    GLuint outputh, outputv;
    
    GLuint program_id;
    
    GLint input_texture_id, step_id;
    
public:
    BlurPass(int width, int height);

    void pass(GLuint inputTexture);
    
    GLuint outputTexture() const override;
    
    virtual ~BlurPass();
    
private:
    void init_blur(GLuint &fbo, GLuint &output_texture);
};


#endif //DEFERRED_BLURPASS_H

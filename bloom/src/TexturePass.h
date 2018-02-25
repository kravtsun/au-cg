#ifndef DEFERRED_TEXTUREPASS_H
#define DEFERRED_TEXTUREPASS_H

#include "PassCommon.h"

struct TexturePass : public Pass {
    GLuint program_id;
    GLint texture_id;
    
public:
    TexturePass(int width, int height);
    
    void pass(GLuint texture) const;
    
    GLuint outputTexture() const override;
    
    virtual ~TexturePass();
};

#endif //DEFERRED_TEXTUREPASS_H

#ifndef SALUTE_TEXTURE_H
#define SALUTE_TEXTURE_H

#include <GL/glew.h>

struct Texture final {
    Texture();
    
    ~Texture();
    
    operator GLuint() const;
    
    void bind(GLenum target=GL_TEXTURE_2D) const;
    
    void reset() const;
private:
    GLuint texture;
};


#endif //SALUTE_TEXTURE_H

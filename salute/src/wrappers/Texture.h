#ifndef SALUTE_TEXTURE_H
#define SALUTE_TEXTURE_H

#include <GL/glew.h>

struct Texture final {
    Texture();
    
    ~Texture();
    
    operator GLuint() const;
    
    void bind(GLenum target=GL_TEXTURE_2D) const;
    
    void bind_as_input(GLint uniform_id, GLuint texture_unit_id=0, GLenum target=GL_TEXTURE_2D);
    
    void reset() const;
private:
    GLuint texture;
};


#endif //SALUTE_TEXTURE_H

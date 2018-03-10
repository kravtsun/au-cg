#ifndef SALUTE_FRAMEBUFFER_H
#define SALUTE_FRAMEBUFFER_H

#include <GL/glew.h>
#include "Texture.h"

struct Framebuffer final {
    Framebuffer();
    
    ~Framebuffer();
    
    operator GLuint() const;
    
    void bind(GLenum target=GL_FRAMEBUFFER) const;
    
    void bind_output_texture(Texture &texture) const;

private:
    GLuint fbo;
};


#endif //SALUTE_FRAMEBUFFER_H

#ifndef SALUTE_TEXTURE_H
#define SALUTE_TEXTURE_H

#include <string>
#include <GL/glew.h>

struct Texture final {
    Texture(int width, int height);
    
    explicit Texture(const std::string &filename);
    
    ~Texture();
    
    operator GLuint() const;
    
    void bind_data(int width, int height, unsigned char *data, int nchannels=3, GLenum type=GL_UNSIGNED_BYTE) const;
    
    void bind(GLenum target=GL_TEXTURE_2D) const;
    
    void bind_as_input(GLint uniform_id, GLuint texture_unit_id=0, GLenum target=GL_TEXTURE_2D);
    
    void reset() const;
    
    static unsigned char *load_bmp(const std::string &filename, int &width, int &height);

private:
    GLuint texture;
    
    void init_and_bind(int width, int height);
};

#endif //SALUTE_TEXTURE_H

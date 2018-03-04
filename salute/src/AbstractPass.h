#ifndef SALUTE_PASSCOMMON_H
#define SALUTE_PASSCOMMON_H

#include <GL/glew.h>

struct AbstractPass {
    AbstractPass(int width, int height);
    
    int get_width() const { return width; }
    
    int get_height() const { return height; }
    
    virtual void pass() = 0;
    
    virtual GLuint output_texture() const = 0;
    
    virtual ~AbstractPass();

protected:
    // helpers for inheritors.
    // TODO make a base class for passes rendering into a separate framebuffer with (one?) output texture
    static void init_and_bind_empty_texture(GLuint &texture, int width, int height);
    
    void init_framebuffer_with_output_texture(GLuint &fbo, GLuint &color_texture);
    
private:
    const int width, height;
};

#define PASS_UNIFORM_3F(id, value) \
    glUniform3f(id, (value).x, (value).y, (value).z);

#define PASS_UNIFORM_MAT4(id, value)                                \
    do {                                                            \
        const glm::mat4 stable_value = (value);                     \
        glUniformMatrix4fv(id, 1, GL_FALSE, &(stable_value)[0][0]); \
    } while (false);

#endif //SALUTE_PASSCOMMON_H

#ifndef DEFERRED_PASSCOMMON_H
#define DEFERRED_PASSCOMMON_H

#include <GL/glew.h>

void init_texture(int width, int height, GLuint texture, int attachment=-1);

#define PASS_UNIFORM_3F(id, value) \
    glUniform3f(id, (value).x, (value).y, (value).z);

#define PASS_UNIFORM_MAT4(id, value)                                \
    do {                                                            \
        const glm::mat4 stable_value = (value);                     \
        glUniformMatrix4fv(id, 1, GL_FALSE, &(stable_value)[0][0]); \
    } while (false);

struct Pass {
    Pass(int width, int height);
    
    int getWidth() {
        return width;
    }
    
    int getHeight() {
        return height;
    }
    
    virtual ~Pass();

protected:
    
    void drawTexture() const;
private:
    const int width, height;
    
    GLuint quad_VertexArrayID, quad_vertexbuffer;
};

#endif //DEFERRED_PASSCOMMON_H

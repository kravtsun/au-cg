#ifndef SALUTE_VERTEXARRAY_H
#define SALUTE_VERTEXARRAY_H

#include <GL/glew.h>

struct VertexArray final {
    VertexArray();
    
    operator GLuint() const;
    
    void bind() const;
    
    ~VertexArray();
    
private:
    GLuint vao;
};

#endif //SALUTE_VERTEXARRAY_H

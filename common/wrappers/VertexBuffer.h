#ifndef SALUTE_VERTEXBUFFER_H
#define SALUTE_VERTEXBUFFER_H

#include <GL/glew.h>

struct VertexBuffer final {
    VertexBuffer();
    
    operator GLuint() const;
    
    void bind(GLenum target=GL_ARRAY_BUFFER) const;
    
    void bind_vertex_attrib(GLuint index, GLuint element_size, GLsizei stride=0);
    
    void unbind_vertex_attrib();
    
    void data(const void *ptr, size_t size, GLenum target=GL_ARRAY_BUFFER, GLenum hint=GL_STATIC_DRAW) const;
    
    ~VertexBuffer();

private:
    GLuint vbo;
    int vertex_attrib_index;
};

#endif //SALUTE_VERTEXBUFFER_H

#include <cassert>
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &vbo);
}

VertexBuffer::operator GLuint() const {
    return vbo;
}

void VertexBuffer::bind(GLenum target) const {
    glBindBuffer(target, vbo);
}

void VertexBuffer::bind_vertex_attrib(GLuint index, GLuint element_size, GLsizei stride) {
    glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(
            index,
            element_size,
            GL_FLOAT,
            GL_FALSE,
            stride,
            nullptr
    );
    vertex_attrib_index = index;
}

void VertexBuffer::unbind_vertex_attrib() {
    assert(vertex_attrib_index >= 0);
    glDisableVertexAttribArray(vertex_attrib_index);
}

void VertexBuffer::data(const void *ptr, size_t size, GLenum target, GLenum hint) const {
    bind(target);
    glBufferData(target, size, ptr, hint);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &vbo);
}

#include "VertexArray.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &vao);
}

VertexArray::operator GLuint() const {
    return vao;
}

void VertexArray::bind() const {
    glBindVertexArray(vao);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &vao);
}

#include "Program.h"
#include "shader.h"

Program::Program(const char *vertex_shader, const char *fragment_shader) {
    program_id = load_shaders(vertex_shader, fragment_shader);
}

Program::~Program() {
    glDeleteProgram(program_id);
}

Program::operator GLuint() const {
    return program_id;
}

void Program::use() const {
    glUseProgram(program_id);
}

GLint Program::location(const char *name) const {
    return glGetUniformLocation(program_id, name);
}

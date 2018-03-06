#ifndef SALUTE_PROGRAMWRAPPER_H
#define SALUTE_PROGRAMWRAPPER_H

#include <GL/glew.h>

struct Program final {
    Program(const char *vertex_shader, const char *fragment_shader);
    
    operator GLuint() const;
    
    void use() const;
    
    GLint location(const char *name) const;
    
    ~Program();

private:
    GLuint program_id;
};

#endif //SALUTE_PROGRAMWRAPPER_H

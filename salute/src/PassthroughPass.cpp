#include <cassert>
#include <stdexcept>
#include "PassthroughPass.h"
#include "shader.h"

static const GLfloat quadz = 1.0f;
static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, quadz,
        1.0f, -1.0f, quadz,
        -1.0f, 1.0f, quadz,
        -1.0f, 1.0f, quadz,
        1.0f, -1.0f, quadz,
        1.0f, 1.0f, quadz,
};

PassthroughPass::PassthroughPass(const int width, const int height, bool direct)
    : AbstractPass(width, height)
    , is_direct(direct)
{
    if (is_direct) {
        program_id = load_shaders("pass_texture.vsh", "pass_texture.fsh");
        pass_texture_id = glGetUniformLocation(program_id, "input_texture");
    }
    
    // TODO a separate vertex array object?
    
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
}

void PassthroughPass::pass() {
    check_input_texture_set();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glUseProgram(program_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, input_texture);
    glUniform1i(pass_texture_id, 0);

//    glEnable(GL_BLEND);
//    glBlendFunc(GL_ONE, GL_ONE);
    draw_quad();
}

PassthroughPass::~PassthroughPass() {
    glDeleteBuffers(1, &quad_vertexbuffer);
    if (!is_direct) {
        glDeleteProgram(program_id);
    }
}

GLuint PassthroughPass::output_texture() const {
    throw std::logic_error("PassthroughPass is supposed to send output to the default framebuffer only");
}

void PassthroughPass::draw_quad() const {
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            nullptr
    );
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
}

void PassthroughPass::check_input_texture_set(const std::string &caller) const {
    if (get_input_texture() == static_cast<GLuint>(-1)) {
        throw std::logic_error("Error in " + caller + ": Input texture not set.");
    }
}

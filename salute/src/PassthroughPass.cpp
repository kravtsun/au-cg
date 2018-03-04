#include <cassert>
#include <stdexcept>
#include "PassthroughPass.h"

#include "wrappers/AllWrappers.h"

static const GLfloat quadz = 1.0f;
static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, quadz,
        1.0f, -1.0f, quadz,
        -1.0f, 1.0f, quadz,
        -1.0f, 1.0f, quadz,
        1.0f, -1.0f, quadz,
        1.0f, 1.0f, quadz,
};

PassthroughPass::PassthroughPass(int width, int height, bool direct)
    : AbstractPass(width, height)
    , is_direct(direct)
{
    if (is_direct) {
        program = std::make_shared<Program>("pass_texture.vsh", "pass_texture.fsh");
        pass_texture_id = glGetUniformLocation(*program, "input_texture");
    }
    
    // TODO a separate vertex array object?
    
    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);
    
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
}

void PassthroughPass::pass() {
    check_input_texture_set();
    program->use();
    glActiveTexture(GL_TEXTURE0);
    get_input_texture()->bind();
    glUniform1i(pass_texture_id, 0);
    
    draw_quad();
}

PassthroughPass::~PassthroughPass() {
    glDeleteBuffers(1, &quad_vertexbuffer);
    glDeleteVertexArrays(1, &quad_vao);
}

TextureWrapper PassthroughPass::output_texture() const {
    throw std::logic_error("PassthroughPass is supposed to send output to the default framebuffer only");
}

void PassthroughPass::draw_quad() const {
    glBindVertexArray(quad_vao);
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
    if (get_input_texture() == nullptr) {
        throw std::logic_error("Error in " + caller + ": Input texture not set.");
    }
}

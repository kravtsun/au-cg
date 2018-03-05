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
    , quad_vao(new VertexArray())
    , quad_vertexbuffer(new VertexBuffer())
{
    if (is_direct) {
        program = std::make_shared<Program>("pass_texture.vsh", "pass_texture.fsh");
        pass_texture_id = glGetUniformLocation(*program, "input_texture");
    }
    
    quad_vao->bind();
    quad_vertexbuffer->data(g_quad_vertex_buffer_data, sizeof(g_quad_vertex_buffer_data));
}

void PassthroughPass::pass() {
    check_input_texture_set("PassthroughPass");
    program->use();
    get_input_texture()->bind_as_input(pass_texture_id, 0);
    draw_quad();
}

void PassthroughPass::set_input_texture(TextureWrapper new_input_texture) {
    input_texture = new_input_texture;
}

PassthroughPass::~PassthroughPass() = default;

TextureWrapper PassthroughPass::output_texture() const {
    throw std::logic_error("PassthroughPass is supposed to send output to the default framebuffer only");
}

void PassthroughPass::draw_quad() const {
    quad_vao->bind();
    quad_vertexbuffer->bind_vertex_attrib(0, 3);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    quad_vertexbuffer->unbind_vertex_attrib();
}

TextureWrapper PassthroughPass::get_input_texture() const {
    return input_texture;
}

void PassthroughPass::check_input_texture_set(const std::string &caller) const {
    if (get_input_texture() == nullptr) {
        throw std::logic_error("Error in " + caller + ": Input texture not set.");
    }
}

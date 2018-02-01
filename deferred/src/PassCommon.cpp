#include <cassert>
#include "PassCommon.h"

void init_texture(const int width, const int height, const GLuint texture, const int attachment) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    
    // Poor filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    if (attachment != -1) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum) attachment, GL_TEXTURE_2D, texture, 0);
    }
}

void Pass::init_output_texture(GLuint &texture) {
    glGenTextures(1, &texture);
    init_texture(getWidth(), getHeight(), texture, GL_COLOR_ATTACHMENT0);
    
    const GLenum DrawBuffers[] = {
            GL_COLOR_ATTACHMENT0
    };
    const GLsizei drawBuffersCount = sizeof(DrawBuffers) / sizeof(DrawBuffers[0]);
    static_assert(drawBuffersCount == 1, "drawBuffersCount == 1");
    glDrawBuffers(drawBuffersCount, DrawBuffers);
    
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(fbo_status == GL_FRAMEBUFFER_COMPLETE);
}

static const GLfloat quadz = 1.0f;
static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, quadz,
        1.0f, -1.0f, quadz,
        -1.0f, 1.0f, quadz,
        -1.0f, 1.0f, quadz,
        1.0f, -1.0f, quadz,
        1.0f, 1.0f, quadz,
};

Pass::Pass(int width, int height)
        : width(width)
        , height(height)
{
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &quad_VertexArrayID);
    glBindVertexArray(quad_VertexArrayID);
}

void Pass::drawTexture() const {
    // 1rst attribute buffer : vertices
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

Pass::~Pass() {
    glGenVertexArrays(1, &quad_VertexArrayID);
    glDeleteBuffers(1, &quad_vertexbuffer);
}

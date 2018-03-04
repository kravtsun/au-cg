#include <cassert>
#include "AbstractPass.h"

AbstractPass::AbstractPass(int width, int height)
        : width(width)
        , height(height)
{}

AbstractPass::~AbstractPass() = default;

void AbstractPass::init_and_bind_empty_texture(GLuint &texture, int width, int height) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    
    // Poor filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void AbstractPass::init_framebuffer_with_output_texture(GLuint &fbo, GLuint &color_texture) {
    init_and_bind_empty_texture(color_texture, get_width(), get_height());
    
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

//    glBindTexture(GL_TEXTURE_2D, depth_texture);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

//    glGenRenderbuffers(1, &color_buffer);
//    glBindRenderbuffer(GL_RENDERBUFFER, color_buffer);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_buffer);
//    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(fbo_status == GL_FRAMEBUFFER_COMPLETE);
}

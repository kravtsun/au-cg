#include <stdexcept>
#include "Framebuffer.h"

Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &fbo);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &fbo);
}

Framebuffer::operator GLuint() const {
    return fbo;
}

void Framebuffer::bind(GLenum target) const {
    glBindFramebuffer(target, fbo);
}

void Framebuffer::bind_output_texture(Texture &texture) const {
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbo_status != GL_FRAMEBUFFER_COMPLETE) {
        throw std::logic_error("Failed to complete framebuffer");
    }
}

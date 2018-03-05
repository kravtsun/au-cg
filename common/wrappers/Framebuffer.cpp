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

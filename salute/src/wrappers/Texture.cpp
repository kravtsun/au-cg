#include <cassert>
#include "Texture.h"

Texture::Texture() {
    glGenTextures(1, &texture);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);
}

Texture::operator GLuint() const {
    return texture;
}

void Texture::bind(GLenum target) const {
    glBindTexture(target, texture);
}

void Texture::reset() const {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(fbo_status == GL_FRAMEBUFFER_COMPLETE);
    glClear(GL_COLOR_BUFFER_BIT);
}

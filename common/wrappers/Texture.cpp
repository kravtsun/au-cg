#include <stdexcept>
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

void Texture::bind_as_input(GLint uniform_id, GLuint texture_unit_id, GLenum target) {
    glActiveTexture(GL_TEXTURE0 + texture_unit_id);
    bind(target);
    glUniform1i(uniform_id, texture_unit_id);
}

void Texture::reset() const {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbo_status != GL_FRAMEBUFFER_COMPLETE) {
        throw std::logic_error("Failed to complete framebuffer");
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

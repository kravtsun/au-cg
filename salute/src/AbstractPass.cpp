#include <stdexcept>
#include "AbstractPass.h"
#include "wrappers/AllWrappers.h"

AbstractPass::AbstractPass(int width, int height)
        : width(width)
        , height(height)
{}

AbstractPass::~AbstractPass() = default;

void AbstractPass::init_and_bind_empty_texture(TextureWrapper &texture, int width, int height) {
    texture = std::make_shared<Texture>();
    texture->bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void AbstractPass::init_framebuffer_with_output_texture(FramebufferWrapper &fbo, TextureWrapper &color_texture) const {
    init_and_bind_empty_texture(color_texture, get_width(), get_height());
    
    fbo = std::make_shared<Framebuffer>();
    fbo->bind();
    glViewport(0, 0, get_width(), get_height());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *color_texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    

    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbo_status != GL_FRAMEBUFFER_COMPLETE) {
        throw std::logic_error("Failed to complete framebuffer");
    }
}

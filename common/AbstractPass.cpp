#include <cassert>
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
    texture->bind(GL_TEXTURE_2D);
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

unsigned char *AbstractPass::load_bmp(const std::string &filename, int &width, int &height) {
    constexpr size_t HEADER_SIZE = 54;
    char header[HEADER_SIZE];
    
    FILE *file = fopen(filename.c_str(), "rb");
    if (!file){
        throw std::logic_error("Failed to open image from file: " + filename);
    }
    
    if (fread(header, 1, HEADER_SIZE, file) != HEADER_SIZE || header[0] != 'B' || header[1] != 'M') {
        throw std::logic_error("Bad BMP: " + filename);
    }

//    auto dataPos = *(const int *)&(header[0x0A]);
//    if (dataPos == 0) {
//        dataPos = 54;
//    }
    auto image_size = *reinterpret_cast<const int *>(&header[0x22]);
    width = *reinterpret_cast<const int *>(&header[0x12]);
    height = *reinterpret_cast<const int *>(&header[0x16]);
    
    auto const expected_size = width * height * 3;
    if (image_size == 0) {
        image_size = expected_size;
    } else {
        if (image_size != expected_size) {
            throw std::logic_error("Unexpected BMP format in " + filename + ": image_size=" + std::to_string(image_size)
                                   + " != " + std::to_string(expected_size));
        }
    }
    
    auto data = new unsigned char [image_size];
    assert(image_size > 0);
    fread(data, 1, static_cast<size_t>(image_size), file);
    fclose(file);
    return data;
}

TextureWrapper AbstractPass::load_bmp_texture(const std::string &filename) {
    int width, height;
    auto data = load_bmp(filename, width, height);
    TextureWrapper texture;
    init_and_bind_empty_texture(texture, width, height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    
    delete[] data;
    return texture;
}


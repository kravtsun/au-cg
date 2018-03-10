#include <iostream>
#include <stdexcept>
#include <cassert>
#include "Texture.h"

Texture::Texture(const std::string &filename) {
    int width, height;
    auto data = load_bmp(filename, width, height);
    glGenTextures(1, &texture);
    init_and_bind(width, height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    delete[] data;
}

Texture::Texture(int width, int height) {
    glGenTextures(1, &texture);
    init_and_bind(width, height);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);
}

Texture::operator GLuint() const {
    return texture;
}

void Texture::bind_data(int width, int height, unsigned char *data) const {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
}

void Texture::bind(GLenum target) const {
    glBindTexture(target, texture);
}

void Texture::init_and_bind(int width, int height) {
    bind(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

unsigned char *Texture::load_bmp(const std::string &filename, int &width, int &height) {
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
    width = *reinterpret_cast<const std::int32_t *>(&header[0x12]);
    height = *reinterpret_cast<const std::int32_t *>(&header[0x16]);
    assert(height >= 0);
    
    auto const expected_size = width * height * 3;
    if (image_size == 0) {
        image_size = expected_size;
    } else {
        if (image_size != expected_size) {
            if (expected_size < image_size) {
                image_size = expected_size;
            } else {
                throw std::logic_error(
                        "Unexpected BMP format in " + filename + ": image_size=" + std::to_string(image_size)
                        + " != " + std::to_string(expected_size));
            }
        }
    }
    
    auto data = new unsigned char [image_size];
    assert(image_size > 0);
    auto bytes_read = fread(data, 1, static_cast<size_t>(image_size), file);
    if (bytes_read != image_size) {
        throw std::logic_error("Failed to read BMP data from file: " + filename);
    }
    fclose(file);
    return data;
}

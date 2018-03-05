
#if _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <string>
#include <stdexcept>
#include <cassert>

#include "SkyPass.h"
#include "wrappers/AllWrappers.h"

SkyPass::SkyPass(int width, int height, const std::string &filename)
        : PassthroughPass(width, height, false)
        , program_id(new Program("pass_texture.vsh", "sky.fsh"))
{
    bias_id = glGetUniformLocation(*program_id, "bias");
    input_texture_id = glGetUniformLocation(*program_id, "input_texture");
    input_texture = load_bmp_texture(filename);
    init_framebuffer_with_output_texture(fbo, color_texture);
}

TextureWrapper SkyPass::load_bmp_texture(const std::string &filename) {
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
    auto const width = *reinterpret_cast<const int *>(&header[0x12]);
    auto const height = *reinterpret_cast<const int *>(&header[0x16]);
    
    if (image_size == 0) {
        image_size = width * height * 3;
    }
    
    auto data = new unsigned char [image_size];

    assert(image_size > 0);
    fread(data, 1, static_cast<size_t>(image_size), file);
    fclose(file);
    
    TextureWrapper texture;
    init_and_bind_empty_texture(texture, width, height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    
    delete[] data;
    return texture;
}

SkyPass::~SkyPass() = default;

void SkyPass::pass() {
    program_id->use();
    input_texture->bind_as_input(input_texture_id);
    glUniform2f(bias_id, step.x, step.y);
    step.x += 0.25f / get_width();
    
    if (step.x > 1) {
        step.x -= 1;
    }
    
    fbo->bind();
    input_texture->bind_as_input(input_texture_id, 0);
    glViewport(0, 0, get_width(), get_height());
    glClear(GL_COLOR_BUFFER_BIT);
    draw_quad();
}

TextureWrapper SkyPass::output_texture() const {
    return color_texture;
}

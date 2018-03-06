#include <map>
#include <algorithm>
#include <cassert>
#include <iostream>

#include "CellsPass.h"
#include "wrappers/AllWrappers.h"

#define RESOURCES_PATH "resources/"
#define PHI 1.6180339887

CellsPass::CellsPass(int width, int height)
        : PassthroughPass(width, height, false)
        , program(new Program("pass_texture.vsh", "pass_texture_selective.fsh"))
{
    std::vector<std::vector<std::string>> cells_lines(5);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 5; ++j) {
            std::string value = "x";
            value += static_cast<char>('0' + i);
            value += static_cast<char>('1' + j);
            cells_lines[j].push_back(value);
        }
    }
    
    std::map<char, uchar*> textures_data;
    for (auto const &lines : cells_lines) {
        for (auto const &line : lines) {
            for (auto const &c : line) {
                if (textures_data.count(c) == 0) {
                    const std::string filename = RESOURCES_PATH + std::string(1, c) + ".bmp";
                    int cell_width, cell_height;
                    textures_data[c] = AbstractPass::load_bmp(filename, cell_width, cell_height);
                    if (cell_width != CellHolder::SYMBOL_TEXTURE_WIDTH || cell_height != CellHolder::SYMBOL_TEXTURE_HEIGHT) {
                        auto const &cell_size_str = std::to_string(cell_width) + ", " + std::to_string(cell_height);
                        throw std::logic_error("Expected texture size: " + cell_size_str);
                    }
                }
            }
        }
    }
    
    const int h = 200;
    const glm::vec2 roi_size{static_cast<int>(h * PHI) / 2, h};
    glm::vec2 roi_pos{30, 350};
    const int cell_padding = 30;
    for (auto const &lines : cells_lines) {
        cell_holders.emplace_back(roi_pos, roi_size, lines, textures_data);
        cell_holders.back().set_yspeed((float)rand() / RAND_MAX / 1000);
        roi_pos.x += roi_size.x + cell_padding;
    }
    
    for (auto char_data : textures_data) {
        delete []char_data.second;
    }
    
    init_framebuffer_with_output_texture(fbo, color_texture);
}

void CellsPass::pass() {
    fbo->bind();
    color_texture->reset();

    for (auto &cell : cell_holders) {
        cell.pass(program);
        draw_quad();
    }
}

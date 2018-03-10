#include <iostream>
#include <map>
#include <algorithm>

#include "CellsPass.h"
#include "wrappers/AllWrappers.h"
#include "resources.h"

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
                    auto const filename = SYMBOL_PATH(c);
                    int cell_width, cell_height;
                    textures_data[c] = Texture::load_bmp(filename, cell_width, cell_height);
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
    glm::vec2 roi_pos{40, 350};
    const int cell_padding = 30;
    for (auto const &lines : cells_lines) {
        cell_holders.emplace_back(roi_pos, roi_size, lines, textures_data);
        cell_holders.back().set_yspeed((float)rand() / RAND_MAX / 1000);
        roi_pos.x += roi_size.x + cell_padding;
    }
    
    for (auto char_data : textures_data) {
        delete []char_data.second;
    }
    
    pass_texture_id = program->location("input_texture");
    ymin_id = program->location("ymin");
    ymax_id = program->location("ymax");
    bgcolor_id = program->location("bgcolor");
    fgcolor_id = program->location("fgcolor");
}

void CellsPass::pass() {
    program->use();
    PASS_UNIFORM_4F(fgcolor_id, fgcolor ? fgcolor->get_color() : glm::vec4(1));
    PASS_UNIFORM_4F(bgcolor_id, bgcolor ? bgcolor->get_color() : glm::vec4(0));
    for (auto &cell: cell_holders) {
        cell.get_texture()->bind_as_input(pass_texture_id);
        glUniform1f(ymin_id, static_cast<GLfloat>(cell.get_ymin()));
        glUniform1f(ymax_id, static_cast<GLfloat>(cell.get_ymax()));
        cell.prepare();
        draw_quad();
    }
}

void CellsPass::set_fgcolor(const ColorSupplierWrapper &new_fgcolor) {
    fgcolor = new_fgcolor;
}

void CellsPass::set_bgcolor(const ColorSupplierWrapper &new_bgcolor) {
    bgcolor = new_bgcolor;
}

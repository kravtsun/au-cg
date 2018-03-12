#include <map>
#include <algorithm>
#include <iterator>

#include "CellsPass.h"
#include "wrappers/AllWrappers.h"
#include "resources.h"
#include "roll/RandomRoll.h"

#define PHI 1.6180339887
#define NCELLS 5
#define NFRAMES 300

CellsPass::CellsPass(int width, int height)
        : PassthroughPass(width, height, false)
        , program(new Program("pass_texture.vsh", "pass_texture_selective.fsh"))
{
    cells_lines.resize(NCELLS);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < NCELLS; ++j) {
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
        cell.step();
        draw_quad();
    }
    
    if (roll != nullptr) {
        auto const ncells = cells_lines.size();
        if (roll->is_exhausted()) {
            roll.reset();
            const double EPS = 1e-7;
            for (size_t i = 0; i < ncells; ++i) {
                auto &cell = cell_holders[i];
                cell.set_yspeed(0);
                
                // stabilize position
                auto const count = cells_lines[i].size();
                auto const step = 1. / count;
                for (size_t j = 0; j < count; ++j) {
                    auto const assumed_position = step * j;
                    if (fabs(cell.get_ymin() - assumed_position) < EPS) {
                        cell.set_ymin(assumed_position);
                    }
                }
            }
        } else {
            for (size_t i = 0; i < ncells; ++i) {
                cell_holders[i].set_yspeed(roll->get_speed(i));
            }
            roll->step();
        }
    }
}

void CellsPass::set_fgcolor(const ColorSupplierWrapper &new_fgcolor) {
    fgcolor = new_fgcolor;
}

void CellsPass::set_bgcolor(const ColorSupplierWrapper &new_bgcolor) {
    bgcolor = new_bgcolor;
}

void CellsPass::start() {
    if (roll != nullptr) {
        return;
    }
    
    std::vector<int> cell_counts;
    std::transform(cells_lines.cbegin(), cells_lines.cend(),
                   std::back_inserter(cell_counts),
                   [](const std::vector<std::string> &cell_lines) { return cell_lines.size(); });
    roll = std::make_shared<RandomRoll>(NFRAMES, cell_counts);
}


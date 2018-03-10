#include <algorithm>
#include <string>
#include <cassert>
#include <cstring>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "CellHolder.h"
#include "wrappers/AllWrappers.h"

CellHolder::CellHolder(const glm::ivec2 &roi_pos,
                       const glm::ivec2 &roi_size,
                       const std::vector<std::string> &lines,
                       const std::map<char, uchar *> &textures_data)
        : Button(roi_pos, roi_size)
        , lines(lines)
        , ysize(static_cast<const GLfloat>(1. / lines.size()))
{
    assert(!lines.empty());
    assert(std::all_of(lines.begin(), lines.end(), [](const std::string &s) { return s.size() == CELL_COUNT; }));
    
    const size_t symbol_data_size = SYMBOL_TEXTURE_HEIGHT * SYMBOL_TEXTURE_WIDTH * 3;
    const size_t line_data_size = CELL_COUNT * symbol_data_size;
    auto *full_bmp = new uchar[lines.size() * line_data_size];

    for (size_t i = 0; i < lines.size(); ++i) {
        auto const &line = lines[i];
        for (size_t j = 0; j < line.size(); ++j) {
            auto data = textures_data.at(line[j]);
            for (size_t r = 0; r < SYMBOL_TEXTURE_HEIGHT; ++r) {
                memcpy(full_bmp +
                               (lines.size() - i - 1) * line_data_size +
                               r * CELL_COUNT * SYMBOL_TEXTURE_WIDTH * 3 +
                               j * SYMBOL_TEXTURE_WIDTH * 3,
                    data + r * SYMBOL_TEXTURE_WIDTH * 3,
                    SYMBOL_TEXTURE_WIDTH * 3
                );
            }
        }
    }

    auto const tex_width = static_cast<GLsizei>(CELL_COUNT * SYMBOL_TEXTURE_WIDTH);
    auto const tex_height = static_cast<GLsizei>(SYMBOL_TEXTURE_HEIGHT * lines.size());
    texture.reset(new Texture(tex_width, tex_height));
    texture->bind_data(tex_width, tex_height, full_bmp);
    delete []full_bmp;
}

void CellHolder::prepare() {
    Button::prepare();
    ymin += yspeed;
    if (ymin > 1) {
        ymin -= 1;
    } else if (ymin < 0) {
        ymin += 1;
    }
}

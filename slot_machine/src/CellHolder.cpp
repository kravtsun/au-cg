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
        : lines(lines)
        , ysize(static_cast<const GLfloat>(1. / lines.size()))
        , roi_pos(roi_pos)
        , roi_size(roi_size)
{
    if (roi_pos.x < 0 || roi_pos.y < 0 || roi_size.x < 0 || roi_size.y < 0) {
        throw std::logic_error("Invalid arguments to CellHolder");
    }
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
    AbstractPass::init_and_bind_empty_texture(texture, tex_width, tex_height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, full_bmp);
    delete []full_bmp;
}

void CellHolder::pass(const ProgramWrapper &program) {
    glViewport(roi_pos.x, roi_pos.y, roi_size.x, roi_size.y);
    program->use();
    const GLint pass_texture_id = program->location("input_texture");
    texture->bind_as_input(pass_texture_id);
    
    const GLint ymin_id = program->location("ymin");
    const GLint ymax_id = program->location("ymax");
    
    glUniform1f(ymin_id, ymin);
    glUniform1f(ymax_id, ymin + ysize);
    
    ymin += yspeed;
    if (ymin > 1) {
        ymin -= 1;
    } else if (ymin < 0) {
        ymin += 1;
    }
}

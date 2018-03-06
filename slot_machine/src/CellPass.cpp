#include <set>
#include <string>
#include <map>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "CellPass.h"
#include "wrappers/AllWrappers.h"

#define CELL_COUNT 3

#define SYMBOL_TEXTURE_WIDTH 300
#define SYMBOL_TEXTURE_HEIGHT 300

#define SYMBOL_WIDTH  100
#define SYMBOL_HEIGHT 200

#define HEIGHT_PADDING 10

#define POS_X 100
#define POS_Y 300

#define ROI_WIDTH (CELL_COUNT * SYMBOL_WIDTH)
#define ROI_HEIGHT ((3 * SYMBOL_HEIGHT) / 2)

#define YMIN_DELTA (-0.001)

#define RESOURCES_PATH "resources/"

typedef unsigned char uchar;

ProgramWrapper CellPass::program;
GLint CellPass::pass_texture_id;
GLint CellPass::ymin_id, CellPass::ymax_id;

std::shared_ptr<PassthroughPass> CellPass::passthrough_pass;
FramebufferWrapper CellPass::fbo;
TextureWrapper CellPass::color_texture;

CellPass::CellPass(const glm::ivec2 &roi_pos, const glm::ivec2 &roi_size, const std::vector<std::string> &lines)
        : lines(lines)
        , ysize(static_cast<const GLfloat>(1. / lines.size()))
        , roi_pos(roi_pos)
        , roi_size(roi_size)
{
    if (roi_pos.x < 0 || roi_pos.y < 0 || roi_size.x < 0 || roi_size.y < 0) {
        throw std::logic_error("Invalid arguments to CellPass");
    }

    if (program == nullptr) {
        program = std::make_shared<Program>("pass_texture.vsh", "pass_texture_selective.fsh");
        pass_texture_id = program->location("input_texture");
        ymin_id = program->location("ymin");
        ymax_id = program->location("ymax");
    }
    std::map<char, uchar*> textures_data;
    assert(!lines.empty());
    assert(std::all_of(lines.begin(), lines.end(), [](const std::string &s) { return s.size() == CELL_COUNT; }));
    for (auto const &line : lines) {
        for (auto const &c : line) {
            if (textures_data.count(c) == 0) {
                const std::string filename = RESOURCES_PATH + std::string(1, c) + ".bmp";
                int width, height;
                textures_data[c] = AbstractPass::load_bmp(filename, width, height);
            }
        }
    }

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

    for (auto char_data : textures_data) {
        delete []char_data.second;
    }

    auto const width = CELL_COUNT * SYMBOL_TEXTURE_WIDTH;
    auto const height = static_cast<const int>(SYMBOL_TEXTURE_HEIGHT * lines.size());
    AbstractPass::init_and_bind_empty_texture(texture, width, height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, full_bmp);
    delete []full_bmp;
}

void CellPass::prepare_pass() {
    fbo->bind();
    color_texture->bind();
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void CellPass::pass() {
//    glEnable(GL_MULTISAMPLE);
    glViewport(roi_pos.x, roi_pos.y, roi_size.x, roi_size.y);
    program->use();
    texture->bind_as_input(pass_texture_id);
    glUniform1f(ymin_id, ymin);
    glUniform1f(ymax_id, ymin + ysize);
    passthrough_pass->draw_quad();
    
    ymin += yspeed;
    if (ymin > 1) {
        ymin -= 1;
    } else if (ymin < 0) {
        ymin += 1;
    }
}

void CellPass::prepare_passthrough(int win_width, int win_height) {
    passthrough_pass = std::make_shared<PassthroughPass>(win_width, win_height, false);
    passthrough_pass->init_framebuffer_with_output_texture(fbo, color_texture);
}

#include <iostream>
#include <sstream>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "FPSRenderPass.h"
#include "wrappers/AllWrappers.h"

#include "resources.h"

#if _WIN32
#include "ft2build.h"
#elif __linux__
#include "freetype2/ft2build.h"
#endif
#include FT_FREETYPE_H

FPSRenderPass::FPSRenderPass(int width, int height, size_t mean_window)
           : AbstractPass(width, height)
           , mean_window(mean_window)
           , last_time(now_time())
{
    program = std::make_shared<Program>("text.vsh", "text.fsh");
    projection_id = program->location("projection");
    color_id = program->location("text_color");
    texture_id = program->location("input_texture");

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        throw std::logic_error("Could not init FreeType Library");
    }

    FT_Face face;
    if (FT_New_Face(ft, FONT_PATH, 0, &face)) {
        throw std::logic_error("ERROR::FREETYPE: Failed to load font");
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto load_texture_for_symbol = [&](char c) {
        if (FT_Load_Char(face, static_cast<FT_ULong>(c), FT_LOAD_RENDER)) {
            throw std::logic_error("ERROR::FREETYTPE: Failed to load Glyph for character: " + std::string(1, c));
        }
        auto const glyph_width = face->glyph->bitmap.width;
        auto const glyph_height = face->glyph->bitmap.rows;
        auto texture = std::make_shared<Texture>(glyph_width, glyph_height);
        texture->bind_data(glyph_width, glyph_height, face->glyph->bitmap.buffer, 1);
        CharacterInfo character = {
            texture,
            glm::ivec2(glyph_width, glyph_height),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLint>(face->glyph->advance.x / 64)
        };
        characters[c] = character;
    };

    for (char c = '0'; c <= '9'; ++c) {
        load_texture_for_symbol(c);
    }
    load_texture_for_symbol(' ');
    load_texture_for_symbol('.');
    load_texture_for_symbol('e');
    load_texture_for_symbol('-');
    load_texture_for_symbol('+');
    load_texture_for_symbol('i');
    load_texture_for_symbol('n');
    load_texture_for_symbol('f');
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    vao = std::make_shared<VertexArray>();
    vao->bind();
    
    vbo = std::make_shared<VertexBuffer>();
    vbo->data(nullptr, sizeof(GLfloat) * 6 * 4, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    vbo->bind_vertex_attrib(0, 4, 4 * sizeof(GLfloat));
}

void FPSRenderPass::pass() {
    static std::string text = "inf";
    ++time_count;
    if (time_count % mean_window == 0) {
        auto const current = now_time();
        auto const seconds_passed = get_seconds(current, last_time) / mean_window;
        auto const FPS = 1 / seconds_passed;
        std::ostringstream ss;
        ss << FPS;
        text = ss.str();
        last_time = now_time();
    }
    display_text(text);
}

TextureWrapper FPSRenderPass::output_texture() const {
    throw std::logic_error("no output texture for FPSRenderPass");
}

void FPSRenderPass::display_text(const std::string &text) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    const glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(get_width()),
                                            0.0f, static_cast<GLfloat>(get_height()));
    program->use();
    PASS_UNIFORM_MAT4(projection_id, projection);
    const glm::vec3 color{0, 1, 0};
    auto const scale = 0.5f;
    auto x = get_width() / 2. - text.size() * scale * 20;
    auto const y = 25.0f;
    glViewport(0, 0, get_width(), get_height());
    program->use();
    vao->bind();
    PASS_UNIFORM_3F(color_id, color);
    
    for (auto const &c : text) {
        auto const &ch = characters.at(c);
        auto const xpos = static_cast<GLfloat>(x + ch.bearing.x * scale);
        auto const ypos = y - (ch.glyph_size.y - ch.bearing.y) * scale;
        
        GLfloat w = ch.glyph_size.x * scale;
        GLfloat h = ch.glyph_size.y * scale;
        GLfloat vertices[6][4] = {
                {xpos,     ypos + h, 0.0, 0.0},
                {xpos,     ypos,     0.0, 1.0},
                {xpos + w, ypos,     1.0, 1.0},
                
                {xpos,     ypos + h, 0.0, 0.0},
                {xpos + w, ypos,     1.0, 1.0},
                {xpos + w, ypos + h, 1.0, 0.0}
        };
        ch.texture->bind_as_input(texture_id);
        vbo->data(vertices, sizeof(vertices), GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += ch.advance * scale;
    }
    glDisable(GL_BLEND);
}

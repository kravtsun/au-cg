#include <iostream>
#include <sstream>
#include <stdexcept>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "FPSRenderPass.h"
#include "wrappers/AllWrappers.h"

#include "ft2build.h"
#include FT_FREETYPE_H


/// Holds all state information relevant to a character as loaded using FreeType
struct CharacterInfo {
    TextureWrapper texture;
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<char, CharacterInfo> characters;

static ProgramWrapper program;
static GLint projection_id;
static GLint color_id;
static GLint texture_id;
static VertexBufferWrapper vbo;

FPSRenderPass::FPSRenderPass(int width, int height)
           : AbstractPass(width, height)
{
    program = std::make_shared<Program>("text.vsh", "text.fsh");
    projection_id = program->location("projection");
    color_id = program->location("textColor");
    texture_id = program->location("input_texture");

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        throw std::logic_error("Could not init FreeType Library");
    }

    FT_Face face;
    if (FT_New_Face(ft, "arial.ttf", 0, &face)) {
        throw std::logic_error("ERROR::FREETYPE: Failed to load font");
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto load_texture = [&](char c) {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            throw std::logic_error("ERROR::FREETYTPE: Failed to load Glyph for character: " + std::string(1, c));
        }
        auto const glyph_width = face->glyph->bitmap.width;
        auto const glyph_height = face->glyph->bitmap.rows;
        auto texture = std::make_shared<Texture>(glyph_width, glyph_height);
        texture->bind_data(glyph_width, glyph_height, face->glyph->bitmap.buffer, 1);
        CharacterInfo character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        characters[c] = character;
    };

    for (char c = '0'; c <= '9'; ++c) {
        load_texture(c);
    }
    load_texture(' ');
    load_texture('.');

    vbo = std::make_shared<VertexBuffer>();
    vbo->data(nullptr, sizeof(GLfloat) * 6 * 4, GL_DYNAMIC_DRAW);
    vbo->bind_vertex_attrib(0, 4, 4 * sizeof(GLfloat));
}

static void RenderText(const std::string &text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    program->use();
    glUniform3f(color_id, color.x, color.y, color.z);

    glActiveTexture(GL_TEXTURE0);
    for (auto const &c : text) {
        auto const &ch = characters[c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        ch.texture->bind_as_input(texture_id);
        //vbo->data(vertices, sizeof(vertices), GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, *vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance * 64) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FPSRenderPass::pass() {
    auto const current_time = clock();
    auto const seconds_passed = static_cast<double>(current_time - last_time) / CLOCKS_PER_SEC;
    auto const FPS = 1 / seconds_passed;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(get_width()), 0.0f, static_cast<GLfloat>(get_height()));
    program->use();
    PASS_UNIFORM_MAT4(projection_id, projection);

    std::ostringstream ss;
    ss << "123.456";
    RenderText(ss.str(),  25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    last_time = clock();
}

TextureWrapper FPSRenderPass::output_texture() const {
    throw std::logic_error("no output texture for FPSRenderPass");
}

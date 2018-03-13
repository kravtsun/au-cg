#ifndef SLOT_MACHINE_FPSRENDERPASS_H
#define SLOT_MACHINE_FPSRENDERPASS_H

#include <chrono>
#include <map>
#include <GL/glew.h>
#include "AbstractPass.h"

using time_point = std::chrono::high_resolution_clock::time_point;

struct FPSRenderPass : AbstractPass {
    FPSRenderPass(int width, int height, size_t mean_window=10);

    void pass() override;

    TextureWrapper output_texture() const override;

private:
    ProgramWrapper program;
    time_point last_time;
    size_t time_count = 0;
    size_t mean_window;
    GLint projection_id;
    GLint color_id;
    GLint texture_id;
    VertexBufferWrapper vbo;
    VertexArrayWrapper vao;
    
    /// Holds all state information relevant to a character as loaded using FreeType
    struct CharacterInfo {
        TextureWrapper texture;

        glm::ivec2 glyph_size;

        // Offset from baseline to left/top of glyph
        glm::ivec2 bearing; 

        // Horizontal offset to advance to next glyph
        GLint advance;      
    };
    std::map<char, CharacterInfo> characters;
    
    void display_text(const std::string &text);
};

static inline double get_seconds(const time_point &end, const time_point &begin) {
    return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1e6;
}

static inline time_point now_time() {
    return std::chrono::high_resolution_clock::now();
}

#endif // SLOT_MACHINE_FPSRENDERPASS_H

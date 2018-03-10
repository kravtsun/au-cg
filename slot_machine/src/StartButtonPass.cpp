#include "StartButtonPass.h"
#include "wrappers/AllWrappers.h"

StartButtonPass::StartButtonPass(int width, int height,
                                 const glm::ivec2 &position, const glm::ivec2 &size)
        : PassthroughPass(width, height, false)
        , start_button(position, size)
        , program(new Program("pass_texture.vsh", "pass_texture_start.fsh"))
{
    pass_texture_id = program->location("input_texture");
    fg_color_id = program->location("fgcolor");
    bg_color_id = program->location("bgcolor");
}

void StartButtonPass::pass() {
    program->use();
    auto const current_fgcolor = fgcolor? fgcolor->get_color() : glm::vec4(1);
    auto const current_bgcolor = bgcolor? bgcolor->get_color() : glm::vec4(0);
    glUniform4fv(fg_color_id, 1, &current_fgcolor[0]);
    glUniform4fv(bg_color_id, 1, &current_bgcolor[0]);
    start_button.get_texture()->bind_as_input(pass_texture_id);
    start_button.prepare();
    draw_quad();
}

void StartButtonPass::set_fgcolor(const ColorSupplierWrapper &new_fgcolor) {
    fgcolor = new_fgcolor;
}

void StartButtonPass::set_bgcolor(const ColorSupplierWrapper &new_bgcolor) {
    bgcolor = new_bgcolor;
}

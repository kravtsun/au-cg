#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D input_texture;
uniform float ymin;
uniform float ymax;

uniform vec4 bgcolor;
uniform vec4 fgcolor;

void main() {
    vec2 uv = UV;
    uv.y = ymin + uv.y * (ymax - ymin);
    if (uv.y > 1) {
        uv.y -= 1;
    }
    vec4 input_color = texture(input_texture, uv);
    color = input_color * fgcolor + (1 - input_color) * bgcolor;
}

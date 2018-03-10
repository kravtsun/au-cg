#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D input_texture;
uniform vec4 fgcolor;
uniform vec4 bgcolor;

void main() {
    vec2 uv = UV;
    vec4 texture_color = texture(input_texture, uv);
    color = texture_color * fgcolor + (vec4(1) - texture_color) * bgcolor;
}

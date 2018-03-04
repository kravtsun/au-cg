#version 330 core

in vec2 UV;

uniform sampler2D input_texture;
uniform sampler2D accumulator_texture;

layout(location = 0) out vec4 color;

void main() {
    vec4 input_color = texture(input_texture, UV);
    vec4 accumulator_color = texture(accumulator_texture, UV);
    color = clamp(accumulator_color + input_color, vec4(0), vec4(1));
}

#version 330 core

in vec2 UV;

uniform sampler2D input_texture;
uniform sampler2D accumulator_texture;

layout(location = 0) out vec4 color;

void main() {
    color = texture(input_texture, UV);
}

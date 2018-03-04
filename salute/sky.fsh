#version 330 core

in vec2 UV;

layout(location = 0) out vec4 color;

uniform sampler2D input_texture;
uniform vec2 bias;

void main() {
    vec2 uv = UV + bias;
    if (uv.x > 1) {
        uv.x -= 1;
    }
    if (uv.y > 1) {
        uv.y -= 1;
    }
    color = texture(input_texture, uv);
}

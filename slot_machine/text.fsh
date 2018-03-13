#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D input_texture;
uniform vec3 text_color;

void main() {    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(input_texture, UV).r);
    color = vec4(text_color, 1.0) * sampled;
}

#version 330 core

in vec2 UV;
out vec3 color;

uniform sampler2D firstTexture;
uniform sampler2D secondTexture;

void main() {
    vec3 first_color = texture(firstTexture, UV).xyz;
    vec3 second_color = texture(secondTexture, UV).xyz;

    color = first_color + second_color;
}

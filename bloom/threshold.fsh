#version 330 core

in vec2 UV;
out vec3 mask;

uniform float threshold;
uniform sampler2D colorTexture;

void main() {
    vec3 color = min(texture(colorTexture, UV).xyz, vec3(1));
    float luminance = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
    if (luminance > threshold) {
        mask = vec3(1, 1, 1);
    } else {
        mask = vec3(0, 0, 0);
    }
}

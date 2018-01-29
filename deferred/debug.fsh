#version 330 core

in vec2 UV;
out vec3 color;

uniform sampler2D myTextureSampler;

void main() {
    // Выходной цвет = цвету текстуры в указанных UV-координатах
    color = texture(myTextureSampler, UV).xyz;
}

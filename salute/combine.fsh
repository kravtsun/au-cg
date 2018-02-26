#version 330 core

in vec2 UV;

uniform sampler2D input_texture;
uniform sampler2D accumulator_texture;

layout(location = 0) out vec4 color;

vec4 blend_func1(in vec4 accumulator_color, in vec4 input_color) {
    accumulator_color *= 0.98;
    vec4 sum_color = accumulator_color;
    if (input_color.a > accumulator_color.a) {
        sum_color = input_color;
    }
    return sum_color;
}

vec4 blend_func2(in vec4 accumulator_color, in vec4 input_color) {
    accumulator_color.xyz *= 0.85;
    vec4 sum_color = accumulator_color + input_color;
//    sum_color.xyz *= 0.9;
    return sum_color;
}

void main() {
    vec4 input_color = texture(input_texture, UV);
    vec4 accumulator_color = texture(accumulator_texture, UV);
    color = blend_func2(accumulator_color, input_color);
}

#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Output data ; will be interpolated for each fragment.
out float texture_color;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform int iterations;
uniform float abs_lim2;

float complex_abs2(vec2 z) {
    return z.x * z.x + z.y * z.y;
}

float complex_abs(vec2 z) {
    return sqrt(complex_abs2(z));
}

vec2 complex_sum(vec2 lhs, vec2 rhs) {
    return vec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

vec2 complex_multiply(vec2 lhs, vec2 rhs) {
    return vec2(lhs.x * rhs.x - lhs.y * rhs.y,
                lhs.x * rhs.y + lhs.y * rhs.x);
}


void main() {
    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);
    float x = gl_Position.x;
    float y = gl_Position.y;

    vec2 z = vec2(0, 0);
    int it = 0;
    float k = 3;
    while (it < iterations && complex_abs2(z) < abs_lim2) {
        vec2 z2 = complex_multiply(z, z);
        z = complex_sum(z2, vec2(x * k, y * k));
        it++;
    }
    texture_color = float(it) / iterations;
}

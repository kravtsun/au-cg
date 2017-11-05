#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Output data ; will be interpolated for each fragment.
out float texture_color;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform int iterations;

void main(){
    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);
//    float x = gl_Position.x;
//    float y = gl_Position.y;

    // fractal code.

    texture_color = 0.5;
}


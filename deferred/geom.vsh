#version 430 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

out vec3 vertexPosition_cameraspace; // position.
out vec3 Normal_cameraspace; // normal.
//out vec3 vertexColor; // unused?

void main() {
    vertexPosition_cameraspace = (V * M * vec4(vertexPosition_modelspace, 1)).xyz;
    Normal_cameraspace = (V * M * vec4(vertexNormal_modelspace, 0)).xyz;
//    vertexColor = vec3(1.0, 0.0, 0.0);

    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);
}

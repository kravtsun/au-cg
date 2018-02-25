#version 430 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

out vec3 vertexPosition_worldspace; // position.
out vec3 Normal_cameraspace; // normal.

void main() {
    vec4 vertexPos = M * vec4(vertexPosition_modelspace, 1);
    vertexPosition_worldspace = vertexPos.xyz / vertexPos.w;
    Normal_cameraspace = (V * M * vec4(vertexNormal_modelspace, 0)).xyz;

    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);
}

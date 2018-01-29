#version 430 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

layout(location = 0) out vec3 vertexPosition_cameraspace; // position.
layout(location = 1) out vec3 Normal_cameraspace; // normal.

void main() {
//    pos = vec3      ( gl_ModelViewMatrix * gl_Vertex ); // transformed point to world space (rather camera space?).
//    n   = normalize ( gl_NormalMatrix * vertexNormal_modelspace);    // transformed n
    vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace, 1)).xyz;
    Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace, 0)).xyz;

    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);
//    texture_color = float(it) / iterations;
}

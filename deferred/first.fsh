#version 430 core

//layout(location = 0) in vec3 pos;
//layout(location = 1) in vec3 n;

in vec3 vertexPosition_cameraspace; // position.
in vec3 Normal_cameraspace; // normal.
//varying vec3 n;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;

void main (void)
{
    position = vertexPosition_cameraspace;
    normal = Normal_cameraspace;
//    vec3 nn = normalize(Normal_cameraspace);
//    gl_FragData[0] = pos;
//    gl_FragData[1] = 0.5 * nn + vec3(0.5);
//    gl_FragData [0] = vec4 (pos, gl_FragDepth);
//    gl_FragData [1] = vec4 (0.5 * nn + vec3(0.5), 1.0);
}

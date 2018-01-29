#version 430 core

//layout(location = 0) in vec3 pos;
//layout(location = 1) in vec3 n;

in vec3 vertexPosition_cameraspace; // position.
in vec3 Normal_cameraspace; // normal.
//in vec3 vertexColor;
//varying vec3 n;

uniform vec3 diffuse_color;
uniform vec3 ambient_color;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 diffuse;
layout(location = 3) out vec3 ambient;

void main (void)
{
    position = vertexPosition_cameraspace;
    normal = Normal_cameraspace;
    diffuse = diffuse_color;
    ambient = ambient_color;
//    color = vertexColor;
//    vec3 nn = normalize(Normal_cameraspace);
//    gl_FragData[0] = pos;
//    gl_FragData[1] = 0.5 * nn + vec3(0.5);
//    gl_FragData [0] = vec4 (pos, gl_FragDepth);
//    gl_FragData [1] = vec4 (0.5 * nn + vec3(0.5), 1.0);
}

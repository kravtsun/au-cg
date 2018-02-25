#version 430 core

in vec3 vertexPosition_worldspace; // position.
in vec3 Normal_cameraspace; // normal.

uniform vec3 diffuse_color;
uniform vec3 ambient_color;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 diffuse;
layout(location = 3) out vec3 ambient;

void main (void)
{
    position = vertexPosition_worldspace;
    normal = Normal_cameraspace;
    diffuse = diffuse_color;
    ambient = ambient_color;
}

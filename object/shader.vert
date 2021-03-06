#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

out vec3 Position_worldspace;
//out vec3 Position_modelspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

void main(){
	// Output position of the vertex, in clip space.
	gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);

	vec4 vertPos4 = M * vec4(vertexPosition_modelspace,1);
	Position_worldspace = vertPos4.xyz / vertPos4.w;
//	Position_modelspace = vertexPosition_modelspace;

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
    vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace, 1)).xyz;
    vec3 cameraPosition_cameraspace = vec3(0, 0, 0);
    EyeDirection_cameraspace = cameraPosition_cameraspace - vertexPosition_cameraspace;

	// Normal of the the vertex, in camera space
	Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace, 0)).xyz;
	// Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
}


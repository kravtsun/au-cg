#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

out vec3 vertPos;
out vec3 normalInterp;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

void main(){
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);

	// Position of the vertex, in worldspace : M * position
	vec4 vertPos4 = M * vec4(vertexPosition_modelspace,1);
	vertPos = vec3(vertPos4) / vertPos4.w;

	// Normal of the the vertex, in camera space
	normalInterp = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
}


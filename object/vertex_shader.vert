#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec4 ShadowCoord;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

uniform vec3 lightPos;

uniform mat4 DepthBiasMVP;

void main(){
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);

	// Position of the vertex, in worldspace.
	vec4 vertPos4 = M * vec4(vertexPosition_modelspace,1);
	Position_worldspace = vertPos4.xyz / vertPos4.w;

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
    vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace, 1)).xyz;
    vec3 cameraPosition_cameraspace = vec3(0, 0, 0);
    EyeDirection_cameraspace = cameraPosition_cameraspace - vertexPosition_cameraspace;

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 LightPosition_cameraspace = ( V * vec4(lightPos, 1)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
//    LightDirection_cameraspace = (V * vec4(lightPos, 0)).xyz;

	// Normal of the the vertex, in camera space
	Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace, 0)).xyz;
	// Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

	ShadowCoord = DepthBiasMVP * vec4(vertexPosition_modelspace, 1);
}


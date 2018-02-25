#version 330 core

layout(location = 0) in vec3 squareVertices;
//layout(location = 1) in vec4 xyzs; // Position of the center of the particle and size of the square
//layout(location = 2) in vec4 color;
//layout(location = 3) in vec3 speed_time;
layout(location = 1) in vec3 speed;

//out vec2 UV;
out vec2 fragment_coords;
out vec3 particle_color;

uniform vec3 ParticleStart_worldspace;
uniform vec3 ParticleColor;
uniform float ParticleSize;
uniform float TimeAfterExplosion;

uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;

// Model-View-Projection matrix, but without the Model
// (the position is in BillboardPos; the orientation depends on the camera)
uniform mat4 VP;


void main()
{
//	float particleSize = xyzs.w; // because we encoded it this way.
//	vec3 particleCenter_wordspace = xyzs.xyz;
    vec3 g = vec3(0, -9.8, 0);
    vec3 particleCenter_worldspace =
        ParticleStart_worldspace
        + speed * TimeAfterExplosion
//        + g * TimeAfterExplosion * TimeAfterExplosion / 2.0f
        ;

	vec3 vertexPosition_worldspace =
		particleCenter_worldspace
		+ CameraRight_worldspace * squareVertices.x * ParticleSize
		+ CameraUp_worldspace * squareVertices.y * ParticleSize
		;

	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);
//    gl_Position = VP * vec4(squareVertices, 1.0f);
//    gl_Position = VP * vec4(0, 0, 0, 1);

//	UV = squareVertices.xy + vec2(0.5, 0.5);
    fragment_coords = squareVertices.xy;
	particle_color = ParticleColor;
	particle_color = vec3(1, 1, 1);
}


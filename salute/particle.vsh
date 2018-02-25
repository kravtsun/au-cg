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

const float seconds_to_decelerate = 2.5;
const float deceleration_coefficient = 1 / seconds_to_decelerate;
const float gravity_coefficient = 1;

const float fade_start_time_normalized = 0.2;
const float fade_speed = 20;

float sqr(in float x) {
    return x * x;
}

void main()
{
//	float particleSize = xyzs.w; // because we encoded it this way.
//	vec3 particleCenter_wordspace = xyzs.xyz;
    vec3 g = vec3(0, -9.8, 0) * gravity_coefficient;

    vec3 speed_deceleration = -speed * deceleration_coefficient;
    if (TimeAfterExplosion > seconds_to_decelerate) {
        speed_deceleration = vec3(0, 0, 0);
    }

//    vec3 current_speed = speed;
//    float k = 30 / seconds_to_decelerate;
    vec3 particleCenter_worldspace =
        ParticleStart_worldspace
        + speed * TimeAfterExplosion
//        + g * sqr(TimeAfterExplosion) / 2.0f
        + (g + speed_deceleration) * sqr(TimeAfterExplosion) / 2.0f
//        + speed * (1 - exp(10 * time_normalized)) / 10.
//        + speed * (1 - exp(-k * TimeAfterExplosion)) / k
        ;

	vec3 vertexPosition_worldspace =
		particleCenter_worldspace
		+ CameraRight_worldspace * squareVertices.x * ParticleSize
		+ CameraUp_worldspace * squareVertices.y * ParticleSize
		;

	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);
//	UV = squareVertices.xy * 0.5 + vec2(0.5, 0.5);
    fragment_coords = squareVertices.xy;

    float time_normalized = TimeAfterExplosion / seconds_to_decelerate;
    float fade_multiplier = 1.0;
    if (time_normalized > fade_start_time_normalized) {
        fade_multiplier = exp(-fade_speed * sqr(time_normalized - fade_start_time_normalized));
    }
	particle_color = ParticleColor * fade_multiplier;
}

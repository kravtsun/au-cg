#version 330 core

//in vec2 UV;
in vec2 fragment_coords; // coords inside a particle's circle (for fading effect).
in vec3 particle_color;
in float fade_multiplier;

layout (location = 0) out vec4 color;

//uniform sampler2D myTextureSampler;

float sqr(in float x) {
    return x * x;
}

void main(){
//	color = texture( myTextureSampler, UV ) * particlecolor;
    color.rgb = particle_color;
    float r = length(fragment_coords);
    color.a = max(0, 1 - r*r);
    color.a *= fade_multiplier;
//    color.xyz *= fade_multiplier; // no need, as alpha is used in combiner.
}

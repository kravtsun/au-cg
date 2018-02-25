#version 330 core

//in vec2 UV;
in vec2 fragment_coords; // coords inside a particle's circle (for fading effect).
in vec3 particle_color;

out vec4 color;

//uniform sampler2D myTextureSampler;

float sqr(in float x) {
    return x * x;
}

void main(){
	// Output color = color of the texture at the specified UV
//	color = texture( myTextureSampler, UV ) * particlecolor;
    color.rgb = particle_color;
    float r = length(fragment_coords);
    color.a = max(0, 1 - r*r);
}

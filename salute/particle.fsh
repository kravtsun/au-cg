#version 330 core

//in vec2 UV;
in vec2 fragment_coords; // coords inside a particle's circle (for fading effect).
in vec4 particle_color;

out vec4 color;

//uniform sampler2D myTextureSampler;

void main(){
	// Output color = color of the texture at the specified UV
//	color = texture( myTextureSampler, UV ) * particlecolor;
    color = particle_color; // TODO make color fading closer to the edge.
}

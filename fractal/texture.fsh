#version 330 core

// Interpolated values from the vertex shaders
in float texture_color;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler1D myTextureSampler;

void main(){
    // Output color = color of the texture at the specified UV
//    color = texture1D(myTextureSampler, texture_color).rgb;
//    color = vec3(0.5, 0.5, 0.5);
    color = vec3(1, 1, 1);
}

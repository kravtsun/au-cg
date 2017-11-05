#version 330 core

// Interpolated values from the vertex shaders
in float texture_color;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler1D myTextureSampler;

void main() {
    // Output color = color of the texture at the specified UV
    float r = texture(myTextureSampler, texture_color).r;
    color = vec3(r, 0, 0);
}

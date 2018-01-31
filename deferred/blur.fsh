#version 330 core

in vec2 UV;
out vec3 color;

uniform vec2 step;
uniform sampler2D inputTexture;

const int N = 7;
const float delta[N] = float[] (
    -3.0,
    -2.0,
    -1.0,
    0.0,
    1.0,
    2.0,
    3.0
);

const float weight[N] = float[] (
    0.015625,
    0.09375,
    0.234375,
    0.3125,
    0.234375,
    0.09375,
    0.015625
);

//const float weight[N] = float[] (
//    0.00598,
//    0.060626,
//    0.241843,
//    0.383103,
//    0.241843,
//    0.060626,
//    0.00598
//);

void main() {
    vec4 sum_color = vec4(0);
	for( int i = 0; i < N; i++ )
	{
	    vec2 coordinates = UV + delta[i] * step;
		sum_color += texture(inputTexture, coordinates) * weight[i];
	}

    color = sum_color.xyz;
}

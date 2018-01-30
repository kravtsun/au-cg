#version 330 core

in vec2 UV;
out vec3 color;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D ambientTexture;

uniform vec3 specular_color;

//uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightAngle;

const float shininess = 16.0;

#define M_PI_2 1.57079632679489661923f	/* pi/2 */

const float lightAngleDelta = M_PI_2 / 5;

float sqr(in float x) {
    return x * x;
}

float light_cone_coefficient(in vec3 position) {
    vec3 lightPosProj = vec3(lightPos.x, position.y, lightPos.z);
    float a = distance(lightPos, lightPosProj);
//    float b = distance(lightPos, position);
    float c = distance(lightPosProj, position);
//    float cos_beta = (a*a + b*b - c*c) / (2 * a * b);
//    float beta = acos(cos_beta);
//    return beta <= lightAngle / 2.0;
    float beta = a > 0 ? atan(c / a) : M_PI_2;
    return beta <= lightAngle / 2.0 ? 1.0 : max(0.0, 1 - (beta - (lightAngle / 2.0)) / lightAngleDelta);
}

void main() {
    vec3 vertexPosition_worldspace = texture(positionTexture, UV).xyz;
    vec3 ambient_color = texture(ambientTexture, UV).xyz;

    vec3 normal = texture(normalTexture, UV).xyz;
    vec3 diffuse_color = texture(diffuseTexture, UV).xyz;

    vec3 cameraPosition_cameraspace = vec3(0, 0, 0);
    vec3 vertexPosition_cameraspace = (V * vec4(vertexPosition_worldspace, 1)).xyz;
    vec3 EyeDirection_cameraspace = cameraPosition_cameraspace - vertexPosition_cameraspace;

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 LightPosition_cameraspace = (V * vec4(lightPos, 1)).xyz;
    vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

    vec3 lightDir = normalize(LightDirection_cameraspace);

    float lambertian = clamp(dot(lightDir, normal), 0.0, 1.0);
    float specular = 0.0;

    if(lambertian > 0.0) {
        vec3 viewDir = normalize(EyeDirection_cameraspace);
        // this is blinn phong
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, shininess);
    }

    // TODO blending.
//    scatteredLight += lambertian * lightColor;
//    reflectedLight += specular * lightColor;
    vec3 scatteredLight = lambertian * lightColor;
    vec3 reflectedLight = specular * lightColor;
    vec3 linear_color = min(
//                            ambient_color +
                            light_cone_coefficient(vertexPosition_worldspace) *
                                (scatteredLight * diffuse_color +
                                reflectedLight * specular_color)
                        , vec3(1));

    color = linear_color;
//    color = ambient_color;
//    color = texture(positionTexture, UV).xyz;
}

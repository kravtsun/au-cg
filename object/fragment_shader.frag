#version 330 core

in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec4 ShadowCoord;

out vec3 color;

uniform mat4 MV;
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 diffuse_color;
uniform vec3 ambient_color;
uniform vec3 specular_color;

uniform vec3 cameraPos;

const float shininess = 16.0;
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space

uniform sampler2DShadow shadowMap;

void main() {
    vec3 normal = normalize(Normal_cameraspace);
//    vec3 lightDir = normalize(lightPos - Position_worldspace);
    vec3 lightDir = normalize(LightDirection_cameraspace);

    float lambertian = clamp(dot(lightDir, normal), 0.0, 1.0);
    float specular = 0.0;

    if(lambertian > 0.0) {
//        vec3 viewDir = normalize(cameraPos - Position_worldspace);
        vec3 viewDir = normalize(EyeDirection_cameraspace);

        int mode = 1;
        if (mode == 1) {
            // this is blinn phong
            vec3 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, shininess);
        } else if(mode == 2) {
            // this is phong (for comparison)
            vec3 reflectDir = reflect(-lightDir, normal);
            float specAngle = max(dot(reflectDir, viewDir), 0.0);
             // note that the exponent is different here
            specular = pow(specAngle, 4.0);
        }
    }

    float bias = 0.005 * tan(acos(lambertian));
//    float visibility = 1.0;
//    if ( texture( shadowMap, ShadowCoord.xy ).z  <  ShadowCoord.z-bias){
//        visibility = 0.5;
//    }

    float visibility = texture( shadowMap, vec3(ShadowCoord.xy, (ShadowCoord.z - bias) / ShadowCoord.w) );
//    visibility = 1.0;
//    float visibility = texture( shadowMap, vec3(ShadowCoord.xy, (ShadowCoord.z) / ShadowCoord.w) );

    vec3 linear_color = 0
                + ambient_color
                + visibility * lambertian * lightColor * diffuse_color
                + visibility * specular * lightColor * specular_color
                ;

    // gamma correction.
//    color = pow(linear_color, vec3(1.0 / screenGamma));
    color = linear_color;
}

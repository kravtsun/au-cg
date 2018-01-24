#version 330 core

//in vec3 Position_modelspace;
in vec3 Position_worldspace; // TODO: remove unused.
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;

out vec3 color;

uniform mat4 M;
uniform mat4 V;
uniform vec3 diffuse_color;
uniform vec3 ambient_color;
uniform vec3 specular_color;

const float shininess = 16.0;

struct LightProperties {
    vec3 color;
    vec3 position;
    mat4 DepthBiasVP;
};

const int lightsCount = 2;
uniform LightProperties Lights[lightsCount];

uniform sampler2DShadow shadowMaps[lightsCount];

void main() {
    vec3 normal = normalize(Normal_cameraspace);

    vec3 scatteredLight = vec3(0);
    vec3 reflectedLight = vec3(0);

    for (int ilight = 0; ilight < lightsCount; ++ilight) {
        vec3 lightColor = Lights[ilight].color;
        vec3 lightPos = Lights[ilight].position;
        mat4 DepthBiasVP = Lights[ilight].DepthBiasVP;

        // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
        vec3 LightPosition_cameraspace = (V * vec4(lightPos, 1)).xyz;
        vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

        vec4 ShadowCoord = DepthBiasVP * vec4(Position_worldspace, 1);

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

        float bias = 0.005 * tan(acos(lambertian));
        vec3 biasedShadowCoord = vec3(ShadowCoord.xy, (ShadowCoord.z - bias) / ShadowCoord.w);

        //    float visibility = 1.0;
        //    if ( texture( shadowMap, ShadowCoord.xy ).z  <  ShadowCoord.z - bias){
        //        visibility = 0.5;
        //    }
        float visibility = texture(shadowMaps[ilight], biasedShadowCoord);

        scatteredLight += visibility * lambertian * lightColor;
        reflectedLight += visibility * specular * lightColor;
    }

    vec3 linear_color = min(ambient_color + scatteredLight * diffuse_color + reflectedLight * specular_color, vec3(1));
    color = linear_color;
}

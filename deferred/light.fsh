#version 330 core

in vec2 UV;
out vec3 color;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D ambientTexture;

//uniform vec3 diffuse_color;
//uniform vec3 ambient_color;
uniform vec3 specular_color;

//uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;
uniform vec3 lightColor;

const float shininess = 16.0;

void main() {
    vec3 position = texture(positionTexture, UV).xyz;
    vec3 normal = texture(normalTexture, UV).xyz;
    vec3 diffuse_color = texture(diffuseTexture, UV).xyz;
    vec3 ambient_color = texture(ambientTexture, UV).xyz;

    vec3 cameraPosition_cameraspace = vec3(0, 0, 0);
    vec3 vertexPosition_cameraspace = position;
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
//    vec3 scatteredLight = lambertian * lightColor;
//    vec3 reflectedLight = specular * lightColor;
//    vec3 linear_color = min(ambient_color + scatteredLight * diffuse_color + reflectedLight * specular_color, vec3(1));
    vec3 linear_color = min(ambient_color + lightColor * (lambertian * diffuse_color + specular * specular_color), vec3(1));
    color = linear_color;
//    color = ambient_color;
//    color = texture(positionTexture, UV).xyz;
}

//
//struct LightProperties {
//    vec3 color;
//    vec3 position;
//    mat4 DepthBiasVP;
//};
//
//const int lightsCount = 2;
//uniform LightProperties Lights[lightsCount];
//
//uniform sampler2DShadow shadowMaps[lightsCount];
//
//void main() {
//    vec3 normal = normalize(Normal_cameraspace);
//
//    vec3 scatteredLight = vec3(0);
//    vec3 reflectedLight = vec3(0);
//
//    for (int ilight = 0; ilight < lightsCount; ++ilight) {
//        vec3 lightColor = Lights[ilight].color;
//        vec3 lightPos = Lights[ilight].position;
//        mat4 DepthBiasVP = Lights[ilight].DepthBiasVP;
//
//        // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
//        vec3 LightPosition_cameraspace = (V * vec4(lightPos, 1)).xyz;
//        vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
//
//        vec4 ShadowCoord = DepthBiasVP * vec4(Position_worldspace, 1);
//
//        vec3 lightDir = normalize(LightDirection_cameraspace);
//
//        float lambertian = clamp(dot(lightDir, normal), 0.0, 1.0);
//        float specular = 0.0;
//
//        if(lambertian > 0.0) {
//            vec3 viewDir = normalize(EyeDirection_cameraspace);
//            // this is blinn phong
//            vec3 halfDir = normalize(lightDir + viewDir);
//            float specAngle = max(dot(halfDir, normal), 0.0);
//            specular = pow(specAngle, shininess);
//        }
//
//        float bias = 0.005 * tan(acos(lambertian));
//        vec3 biasedShadowCoord = vec3(ShadowCoord.xy, (ShadowCoord.z - bias) / ShadowCoord.w);
//
//        //    float visibility = 1.0;
//        //    if ( texture( shadowMap, ShadowCoord.xy ).z  <  ShadowCoord.z - bias){
//        //        visibility = 0.5;
//        //    }
//        float visibility = texture(shadowMaps[ilight], biasedShadowCoord);
//
//        scatteredLight += visibility * lambertian * lightColor;
//        reflectedLight += visibility * specular * lightColor;
//    }
//
//    vec3 linear_color = min(ambient_color + scatteredLight * diffuse_color + reflectedLight * specular_color, vec3(1));
//    color = linear_color;
//}

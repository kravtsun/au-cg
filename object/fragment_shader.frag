#version 330 core

in vec3 vertPos;
in vec3 normalInterp;

out vec3 color;

uniform mat4 MV;
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 diffuse_color;
uniform vec3 ambient_color;
uniform vec3 specular_color;

//uniform float Ka;   // Ambient reflection coefficient
//uniform float Kd;   // Diffuse reflection coefficient
//uniform float Ks;   // Specular reflection coefficient
//uniform float shininessVal; // Shininess

void main() {
  vec3 normal = normalize(normalInterp);
  vec3 lightDir = normalize(lightPos - vertPos);

  float lambertian = max(dot(lightDir, normal), 0.0);
  float specular = 0.0;

  if(lambertian > 0.0) {
    vec3 viewDir = normalize(-vertPos);

    // this is blinn phong
    vec3 halfDir = normalize(lightDir + viewDir);
    float specAngle = max(dot(halfDir, normal), 0.0);
    specular = pow(specAngle, 16.0);

    // this is phong (for comparison)
//    if(mode == 2) {
//      vec3 reflectDir = reflect(-lightDir, normal);
//      specAngle = max(dot(reflectDir, viewDir), 0.0);
//      // note that the exponent is different here
//      specular = pow(specAngle, 4.0);
//    }
  }

  color = 0
        + ambient_color
        + lambertian * lightColor * diffuse_color
        + specular * lightColor * specular_color
        ;
}

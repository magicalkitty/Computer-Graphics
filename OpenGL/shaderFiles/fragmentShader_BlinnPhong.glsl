#version 330 core

layout(location=0) out vec4 fragmentColor;

uniform sampler2D texImage;
uniform vec3 diffuseComponent;
uniform vec3 specularComponent;
uniform float phongExponent;

in vec3 normal;
in vec3 lightDir;
in vec3 viewDir;

void main(void)
{

  vec3 N = normalize(normal.xyz);
  vec3 L = normalize(lightDir.xyz);
  vec3 V = normalize(viewDir.xyz);

  float diff = max(dot(N, L), 0.0);
  vec3 diffuseShading = diffuseComponent.rgb;
  vec3 diffuse = vec3(diffuseShading.r * diff, diffuseShading.g * diff, diffuseShading.b * diff);

  vec3 halfVec = normalize(L + V);
  vec3 specular = specularComponent * pow(max(dot(N, halfVec), 0.0), phongExponent);

  vec3 finalColor = diffuse + specular;

  fragmentColor = vec4(finalColor, 1.0);
}
#version 330 core

layout(location=0) out vec4 fragmentColor;

uniform vec3 diffuseComponent;
uniform vec3 specularComponent;
uniform float phongExponent;

in vec4 normal;
in vec4 lightDir;
in vec4 viewDir;

void main(void)
{

  vec3 N = normalize(normal.xyz);
  vec3 L = normalize(lightDir.xyz);
  vec3 V = normalize(viewDir.xyz);

  float diff = max(dot(N, L), 0.0);
  vec3 diffuseShading = diffuseComponent * diff;

  vec3 halfVec = normalize(L + V);
  float spec = pow(max(dot(N, halfVec), 0.0), phongExponent);
  vec3 specularShading = specularComponent * spec;

  vec3 finalColor = diffuseShading + specularShading;

  fragmentColor = vec4(finalColor, 1.0);
}
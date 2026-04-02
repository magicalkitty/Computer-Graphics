#version 330 core

layout(location=0) out vec4 fragmentColor;

uniform vec3 diffuseComponent;

in vec4 normal;
in vec4 lightDir;

void main(void)
{
  vec3 N = normalize(normal.xyz);
  vec3 L = normalize(lightDir.xyz);

  float diff = max(dot(N, L), 0.0);
  vec3 diffuseShading = diffuseComponent * diff;

  fragmentColor = vec4(diffuseShading, 1.0);
}
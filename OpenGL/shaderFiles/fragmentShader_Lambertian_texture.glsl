#version 330 core

layout(location=0) out vec4 fragmentColor;

uniform sampler2D texImage;
uniform vec3 diffuseComponent;

in vec3 normal;
in vec3 lightDir;
in vec2 texCoord;

void main(void)
{
  vec3 texColor = texture(texImage, texCoord).rgb;
  vec3 N = normalize(normal.xyz);
  vec3 L = normalize(lightDir.xyz);

  float diff = max(dot(N, L), 0.0);
  vec3 diffuseShading = texColor * diff;

  fragmentColor = vec4(diffuseShading, 1.0);
}
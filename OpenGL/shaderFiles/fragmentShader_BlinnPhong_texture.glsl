#version 330 core

layout(location=0) out vec4 fragmentColor;

uniform sampler2D texImage;
uniform vec3 diffuseComponent;
uniform vec3 specularComponent;
uniform float phongExponent;

in vec3 normal;
in vec3 lightDir;
in vec3 viewDir;
in vec2 texCoord;

void main(void)
{
  vec3 texColor = texture(texImage, texCoord).rgb;
  // vec3 texColor = texture(texImage, gl_FragCoord/100).rgb;

  vec3 N = normalize(normal.xyz);
  vec3 L = normalize(lightDir.xyz);
  vec3 V = normalize(viewDir.xyz);

  float diff = max(dot(N, L), 0.0);
  vec3 diffuseShading = texColor * diff;

  vec3 halfVec = normalize(L + V);
  vec3 specular = specularComponent * pow(max(dot(N, halfVec), 0.0), phongExponent);

  vec3 finalColor = diffuseShading + specular;

  fragmentColor = vec4(finalColor, 1.0);
}
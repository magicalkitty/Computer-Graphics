#version 330 core

layout(location=0) out vec4 fragmentColor;
layout(location=1) out vec4 brightColor;

uniform sampler2D texImage;
uniform vec3 diffuseComponent;
uniform float emissiveStrength;

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

  vec3 emissive = texColor * emissiveStrength;
  vec3 color = diffuseShading + emissive;

  fragmentColor = vec4(color, 1.0);

  float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

  if (brightness > 0.8)
    brightColor = vec4(fragmentColor.rgb, 1.0);
  else
    brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
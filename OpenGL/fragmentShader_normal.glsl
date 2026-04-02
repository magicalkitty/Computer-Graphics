#version 330 core

layout(location=0) out vec4 fragmentColor;

in vec3 fragmentNormal;

void main(void)
{
  vec3 color = normalize(fragmentNormal) * 0.5 + 0.5;
  fragmentColor = vec4(color, 1.0);
}
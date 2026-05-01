#version 330 core

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;
layout(location=2) in vec2 in_TexCoord;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

uniform vec4 lightPosWorld;
uniform vec3 cameraPos;

out vec3 normal;
out vec3 lightDir;
out vec3 viewDir;
out vec2 texCoord;

void main(void)
{
  vec4 vertexWorldPos = modelMatrix * vec4(in_Position, 1.0);

  normal = normalize((normalMatrix * vec4(in_Normal, 0.0)).xyz);
  lightDir = lightPosWorld.xyz - vertexWorldPos.xyz;
  viewDir = cameraPos - vertexWorldPos.xyz;

  texCoord = in_TexCoord;

  gl_Position = projMatrix * viewMatrix * vertexWorldPos;
}
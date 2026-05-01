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
out vec2 texCoord;
out vec3 fragPos;
out vec3 viewDir;

void main(void)
{
    vec4 worldPos = modelMatrix * vec4(in_Position, 1.0);

    fragPos = worldPos.xyz;

    normal = normalize((normalMatrix * vec4(in_Normal, 0.0)).xyz);

    viewDir = cameraPos - fragPos;

    texCoord = in_TexCoord;

    gl_Position = projMatrix * viewMatrix * worldPos;
}
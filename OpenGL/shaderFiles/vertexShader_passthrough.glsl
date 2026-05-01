#version 330 core

layout(location=0) in vec3 in_Position;

layout(location=1) in vec3 in_Color;

out vec3 pass_Color;

void main(void)
{
    gl_Position = vec4(in_Position, 1.0);

    pass_Color = in_Color;
}
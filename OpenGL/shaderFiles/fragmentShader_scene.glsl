#version 330 core

layout(location = 0) out vec4 fragmentColor;
layout(location = 1) out vec4 brightColor;

uniform sampler2D texImage;

in vec2 texCoord;

void main()
{
    vec3 color = texture(texImage, texCoord).rgb;

    FragColor = vec4(color, 1.0);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0)
        BrightColor = vec4(color, 1.0);
    else
        BrightColor = vec4(0.0);
}
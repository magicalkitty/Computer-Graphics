#version 330 core
out vec4 fragmentColor;

in vec2 texCoord;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;

void main()
{
    const float gamma = 2.2;

    vec3 hrdColor = texture(scene, texCoord).rgb;
    vec3 bloomColor = texture(bloomBlur, texCoord).rgb;
    
    hrdColor += bloomColor;

    vec3 result = vec3(1.0) - exp(-hrdColor * exposure);
    
    result = pow(result, vec3(1.0 / gamma));
    
    fragmentColor = vec4(result, 1.0);
}
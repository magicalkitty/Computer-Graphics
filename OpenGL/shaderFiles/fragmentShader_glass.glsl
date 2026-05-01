#version 330 core

layout(location=0) out vec4 fragmentColor;

uniform sampler2D texImage;

// OPTIONAL (fake environment if no skybox provided)
uniform samplerCube skybox;

uniform float ior;            // index of refraction (1.52 glass)
uniform float transparency;   // 0 = opaque, 1 = clear

in vec3 normal;
in vec2 texCoord;
in vec3 fragPos;
in vec3 viewDir;

void main(void)
{
    vec3 N = normalize(normal);
    vec3 V = normalize(viewDir);

    // texture = color tint of glass (your OBJ texture drives hue)
    vec3 texColor = texture(texImage, texCoord).rgb;

    // reflection direction
    vec3 R = reflect(-V, N);

    // refraction direction
    vec3 T = refract(-V, N, 1.0 / ior);

    // ==============================
    // ENVIRONMENT (IMPORTANT FIX)
    // ==============================

    vec3 reflected;
    vec3 refracted;

    // If you DO NOT have skybox, use fake environment (prevents black glass)
    reflected = vec3(0.75, 0.85, 1.0);  // sky tint
    refracted = vec3(0.6, 0.75, 0.9);

    // tint refraction with texture color (your requirement)
    refracted *= texColor;

    // ==============================
    // FRESNEL (critical for glass)
    // ==============================
    float F0 = pow((1.0 - ior) / (1.0 + ior), 2.0);
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - max(dot(N, V), 0.0), 5.0);

    // mix reflection + refraction
    vec3 color = mix(refracted, reflected, fresnel);

    // ==============================
    // OUTPUT
    // ==============================
    float alpha = 1.0 - transparency;

    fragmentColor = vec4(color, alpha);
}
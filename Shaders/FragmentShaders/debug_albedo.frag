#version 400 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D albedoSpecTexture;

void main()
{
    FragColor = vec4(texture(albedoSpecTexture, TexCoords).rgb, 1);
}
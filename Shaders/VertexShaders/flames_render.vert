#version 440 core

// Inputs
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aColor;

// Outputs
out vec4 vPosition;
out vec3 vColor;

void main()
{
    vPosition = aPosition;
    vColor = aColor;
}
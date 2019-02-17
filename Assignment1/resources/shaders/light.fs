#version 330 core
out vec4 FragColor;

uniform vec3 intensity;

void main()
{
    FragColor = vec4(intensity,1.0);
}
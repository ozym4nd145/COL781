#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float visibility;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const float density = 0.0015f;
const float gradient = 6.0f;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    
    vec4 positionRelativeCamera = view * vec4(FragPos, 1.0);
    gl_Position = projection * positionRelativeCamera;

    float distanceFromCamera = length(positionRelativeCamera.xyz);
    visibility = exp(-pow((distanceFromCamera*density),gradient));
    // visibility = 1.0f;
    visibility = clamp(visibility, 0.0, 1.0);
}
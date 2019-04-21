#version 330 core

#define NR_TEXTURE 5


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float heightScale;
uniform sampler2D texture_height[NR_TEXTURE];


void main()
{
    vec3 newPos = aPos + length(texture(texture_height[0], aTexCoords).xyz)*heightScale*aNormal;
    // vec3 newPos = aPos;
    FragPos = vec3(model * vec4(newPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal; 
    TexCoords = aTexCoords;

    vec3 T = normalize(vec3(model * vec4(aTangent, 1.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 1.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
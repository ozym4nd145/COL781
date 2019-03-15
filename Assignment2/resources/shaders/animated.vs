#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in int aNumJoints;
layout (location = 4) in ivec3 aJointIndices;
layout (location = 5) in vec3 aJointWeights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

const int MAX_JOINTS = 50; //max joints allowed in a skeleton

// uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 jointTransforms[MAX_JOINTS];

void main()
{
    mat4 model = mat4(0.0f);
    for(int i=0;i<aNumJoints;i++) {
        model += jointTransforms[aJointIndices[i]]*aJointWeights[i];
    }

    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
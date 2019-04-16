#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCenterPos;
layout (location = 2) in vec3 aColor;

out vec3 FragPos;
out vec3 FragColorParticle;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = aCenterPos;
    FragColorParticle = aColor;
    gl_PointSize = 5.0;
    // FragColorParticle = vec3(1.0,0.0,0.0);
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
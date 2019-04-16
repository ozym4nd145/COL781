#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec4 FragColorParticle;

void main()
{
    FragColor = FragColorParticle;
    // FragColor = vec4(0,0.464,0.742,0.2);
    // FragColor = vec4(1.0,0.0,0.0,1.0);
}
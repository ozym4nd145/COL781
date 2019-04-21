#version 330 core

#define NR_TEXTURE 5

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D textures[NR_TEXTURE];

void main()
{ 
    // FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
    FragColor = vec4(vec3(texture(textures[0], TexCoords))*1.5,0.7);
    // float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    // FragColor = vec4(average, average, average, 1.0);
}
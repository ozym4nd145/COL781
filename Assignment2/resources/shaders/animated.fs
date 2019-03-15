#version 330 core

#define NR_TEXTURE 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;

uniform sampler2D texture_diffuse[NR_TEXTURE];
uniform sampler2D texture_specular[NR_TEXTURE];
uniform sampler2D texture_normal[NR_TEXTURE];
uniform sampler2D texture_height[NR_TEXTURE];

uniform int num_texture_diffuse=0;
uniform int num_texture_specular=0;
uniform int num_texture_normal=0;
uniform int num_texture_height=0;

void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);
    // phase 2: point lights
    
    if(num_texture_diffuse > 0) {
        result = vec3(texture(texture_diffuse[0], TexCoords));
    } else {
        result = vec3(0.9,0.2,0.3);
    }
    
    FragColor = vec4(result, 1.0);
}

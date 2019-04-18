#version 330 core

#define NR_TEXTURE 5

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float visibility;

out vec4 FragColor;

uniform sampler2D texture_diffuse[NR_TEXTURE];
uniform sampler2D texture_specular[NR_TEXTURE];
uniform sampler2D texture_normal[NR_TEXTURE];
uniform sampler2D texture_height[NR_TEXTURE];

uniform int num_texture_diffuse=0;
uniform int num_texture_specular=0;
uniform int num_texture_normal=0;
uniform int num_texture_height=0;
uniform int num_point_lights=0;

uniform vec3 skyColor;

void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 result = vec3(0.0);

    if(num_texture_diffuse > 0) {
        // assert(num_texture_diffuse==5);
        vec3 blendMapTexture = vec3(texture(texture_diffuse[0], TexCoords));
        float backgroundAmount = 1 - (blendMapTexture[0]+blendMapTexture[1]+blendMapTexture[2]);
        vec2 tiledCoordinates = TexCoords*100;
        vec3 backgroundTexture = backgroundAmount*vec3(texture(texture_diffuse[1], tiledCoordinates));
        vec3 firstTexture = blendMapTexture[0]*vec3(texture(texture_diffuse[2], tiledCoordinates));
        vec3 secondTexture = blendMapTexture[1]*vec3(texture(texture_diffuse[3], tiledCoordinates));
        vec3 thirdTexture = blendMapTexture[2]*vec3(texture(texture_diffuse[4], tiledCoordinates));
        result = backgroundTexture+firstTexture+secondTexture+thirdTexture;
    } else {
        result = vec3(0.7,0.2,0.0);
    }
    
    FragColor = vec4(result, 1.0);
    FragColor = mix(vec4(skyColor,1.0),FragColor,visibility);
}
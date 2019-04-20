#version 330 core

#define NR_TEXTURE 5
#define NR_POINT_LIGHTS 5

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

out vec4 FragColor;

struct PointLight {
    vec3 position;
    vec3 color;
    vec3 attenuation;
};

struct PointLightResult {
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform sampler2D texture_diffuse[NR_TEXTURE];
uniform sampler2D texture_specular[NR_TEXTURE];
uniform sampler2D texture_normal[NR_TEXTURE];
uniform sampler2D texture_height[NR_TEXTURE];

uniform int num_texture_diffuse=0;
uniform int num_texture_specular=0;
uniform int num_texture_normal=0;
uniform int num_texture_height=0;
uniform int num_point_lights=0;

uniform float shineDamper;
uniform float reflectivity;
uniform vec3 ambientColor;

uniform float heightScale;

PointLightResult CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    if(num_texture_height>0){
        float height =  texture(texture_height[0], texCoords).r;     
        return texCoords - viewDir.xy * (height * heightScale);        
    }
    else{
        return texCoords;
    }
}


void main()
{    
    vec2 texCoords = ParallaxMapping(TexCoords,  normalize(transpose(TBN)*(viewPos - FragPos)));       
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;
    
    // properties
    vec3 norm;
    if(num_texture_normal > 0){
        norm = texture(texture_normal[0], texCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);   
        norm = normalize(TBN * norm);
    }
    else{
        norm = normalize(Normal);
    }
    vec3 viewDir = normalize(viewPos - FragPos);

    vec4 diffuseColor = vec4(0.0);

    if(num_texture_diffuse > 0) {
        diffuseColor = texture(texture_diffuse[0], texCoords);
    } else {
        diffuseColor = vec4(0.2,0.8,0.0,1.0);
    }

    vec3 totalDiffuse = ambientColor;
    vec3 totalSpecular = vec3(0.0);

    for(int i = 0; i < num_point_lights; i++) {
        PointLightResult result = CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
        totalDiffuse += result.diffuse;
        totalSpecular += result.specular;
    }
    
    totalDiffuse = max(totalDiffuse, 0.2);

    FragColor = vec4(totalDiffuse, 1.0) * diffuseColor + vec4(totalSpecular, 1.0);
}


// calculates the color when using a point light.
PointLightResult CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightVector = light.position - fragPos;
    float lightDist = length(lightVector);
    vec3 lightDir = normalize(lightVector);

    float attenuation = light.attenuation.x + light.attenuation.y*lightDist + light.attenuation.z*lightDist*lightDist;

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = (light.color * diff)/attenuation;

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float dampedFactor = pow(max(dot(viewDir, reflectDir), 0.0), shineDamper);
    if(diff == 0.0) {
        dampedFactor = 0.0;
    }
    vec3 specular = (light.color * dampedFactor * reflectivity)/attenuation;

    return PointLightResult(diffuse,specular);
}
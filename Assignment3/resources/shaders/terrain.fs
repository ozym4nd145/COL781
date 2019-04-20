#version 330 core

#define NR_TEXTURE 5
#define NR_POINT_LIGHTS 5

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float visibility;

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
uniform vec3 skyColor;

uniform float seaLevel;
uniform float grassLimit;
uniform float mountainLimit;
uniform float snowLimit;

PointLightResult CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec4 diffuseColor = vec4(0.0);
    vec2 tiledCoordinates = TexCoords*100;

    if(num_texture_diffuse == 1) {
        diffuseColor = texture(texture_diffuse[0], tiledCoordinates);
    } else if(num_texture_diffuse > 1) {
        vec4 grassTexture = texture(texture_diffuse[0], tiledCoordinates);
        vec4 waterTexture = texture(texture_diffuse[1], tiledCoordinates);
        vec4 rockTexture = texture(texture_diffuse[2], tiledCoordinates);
        vec4 snowTexture = texture(texture_diffuse[3], tiledCoordinates);

        float height = FragPos.y;
        float mountainMid = (grassLimit+mountainLimit)/2;
        float snowMid = (mountainLimit+snowLimit)/2;

        // if(height <= seaLevel) {
        //     diffuseColor = waterTexture;
        // } else if (height <= grassLimit) {
        //     diffuseColor = grassTexture;
        // } else if (height <= mountainLimit) {
        //     diffuseColor = rockTexture;
        // } else {
        //     diffuseColor = snowTexture;
        // }
        if(height <= seaLevel) {
            diffuseColor = waterTexture;
        } else if (height <= mountainMid) {
            float ratio = 0.0;
            if(height <= grassLimit) {
                ratio = ((height-seaLevel)/(grassLimit-seaLevel))*0.5;
            } else {
                ratio = 0.5+((height-grassLimit)/(mountainMid-grassLimit))*0.5;
            }
            diffuseColor = mix(grassTexture,rockTexture,ratio);
        } else if (height <= snowMid) {
            float ratio = 0.0;
            if(height <= mountainLimit) {
                ratio = ((height-mountainMid)/(mountainLimit-mountainMid))*0.5;
            } else {
                ratio = 0.5+((height-mountainLimit)/(snowMid-mountainLimit))*0.5;
            }
            diffuseColor = mix(rockTexture,snowTexture,ratio);
        } else {
            diffuseColor = snowTexture;
        }
        // // BLEND MAP
        // vec4 blendMapTexture = texture(texture_diffuse[0], TexCoords);
        // float backgroundAmount = 1 - (blendMapTexture[0]+blendMapTexture[1]+blendMapTexture[2]);
        // vec4 backgroundTexture = backgroundAmount*texture(texture_diffuse[1], tiledCoordinates);
        // vec4 firstTexture = blendMapTexture[0]*texture(texture_diffuse[2], tiledCoordinates);
        // vec4 secondTexture = blendMapTexture[1]*texture(texture_diffuse[3], tiledCoordinates);
        // vec4 thirdTexture = blendMapTexture[2]*texture(texture_diffuse[4], tiledCoordinates);
        // diffuseColor = backgroundTexture+firstTexture+secondTexture+thirdTexture;
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
    
    totalDiffuse = max(totalDiffuse, 0.4);

    FragColor = vec4(totalDiffuse, 1.0) * diffuseColor + vec4(totalSpecular, 1.0);
    FragColor = mix(vec4(skyColor, 1.0), FragColor, visibility);
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
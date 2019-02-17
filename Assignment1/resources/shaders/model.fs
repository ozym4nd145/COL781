#version 330 core

#define NR_POINT_LIGHTS 4
#define NR_TEXTURE 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float shininess;
}; 

struct PointLight {
    vec3 position;
    vec3 intensity;
    vec3 ambient;
};

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

uniform sampler2D texture_diffuse[NR_TEXTURE];
uniform sampler2D texture_specular[NR_TEXTURE];
uniform sampler2D texture_normal[NR_TEXTURE];
uniform sampler2D texture_height[NR_TEXTURE];

uniform int num_texture_diffuse;
uniform int num_texture_specular;
uniform int num_texture_normal;
uniform int num_texture_height;
uniform int num_point_lights;

// function prototypes
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);
    // phase 2: point lights
    for(int i = 0; i < num_point_lights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.ambient;
    vec3 diffuse = light.intensity * diff;
    if(num_texture_diffuse > 0) {
        vec3 diff_material = vec3(texture(texture_diffuse[0], TexCoords));
        diffuse = diffuse * diff_material;
        ambient = ambient * diff_material;
    } else {
        diffuse = diffuse * material.diffuse;
        ambient = ambient * material.ambient;
    }
    
    vec3 specular = light.intensity * spec;
    if(num_texture_specular > 0) {
        specular = specular * vec3(texture(texture_specular[0], TexCoords));
    
    } else {
        specular = specular * material.specular;
    }

    return (ambient + diffuse + specular);
}

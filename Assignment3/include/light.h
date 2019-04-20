#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>

struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 attenuation;
};

class LightScene {
    private:
        glm::vec3 ambientLight;
        std::vector<PointLight> pointLights;
    public:

    LightScene(glm::vec3 ambientLight,const std::vector<PointLight>& pointLights);
    
    void configureLights(Shader shader) const;
};
#include "light.h"



LightScene::LightScene(glm::vec3 ambientLight,const std::vector<PointLight>& pointLights):
    ambientLight{ambientLight},
    pointLights{pointLights.begin(),pointLights.end()}
{

}

void LightScene::configureLights(Shader shader) const {
    shader.setVec3("ambientColor", ambientLight);
    shader.setInt("num_point_lights", pointLights.size());
    for(int idx=0;idx<pointLights.size();idx++) {
        shader.setVec3("pointLights["+std::to_string(idx)+"].position", pointLights[idx].position);
        shader.setVec3("pointLights["+std::to_string(idx)+"].color", pointLights[idx].color);
        shader.setVec3("pointLights["+std::to_string(idx)+"].attenuation", pointLights[idx].attenuation);
    }
}
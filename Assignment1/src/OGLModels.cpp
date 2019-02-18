#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "OGLModels.h"

namespace ogl {

    // Lights
    Lights::Lights(const std::vector<std::pair<Point,Color>>& lights):
        lightModel("/home/oz/Coding/Assignment/COL781/Assignment1/resources/objects/sphere/sphere.obj")
    {
        for(auto light: lights) {
            glm::mat4 model = glm::mat4(1.0);
            auto center = light.first;
            model = glm::translate(model,glm::vec3(center[0],center[1],center[2]));
            model = glm::scale(model,glm::vec3(radius));
            auto intensity = light.second;
            
            transformationMatrices.push_back(model);
            intensities.push_back(glm::vec3(intensity[0],intensity[1],intensity[2]));
            positions.push_back(glm::vec3(center[0],center[1],center[2]));
        }
    }
        
    void Lights::configureLights(Shader shader) const {
        shader.setInt("num_point_lights", positions.size());
        for(int idx=0;idx<positions.size();idx++) {
            shader.setVec3("pointLights["+std::to_string(idx)+"].position", positions[idx]);
            shader.setVec3("pointLights["+std::to_string(idx)+"].ambient", intensities[idx]*(0.1f));
            shader.setVec3("pointLights["+std::to_string(idx)+"].intensity",  intensities[idx]);
        }
    }

    void Lights::DrawLights(Shader shader) const {
        for(int idx=0;idx<positions.size();idx++) {
            shader.setMat4("model",transformationMatrices[idx]);
            shader.setVec3("intensity",intensities[idx]);
            lightModel.Draw(shader);
        }
    }


    // Camera
    CameraModel::CameraModel(const Matrix4f& trans):
        model(glm::mat4(1.0f)),
        cameraModel("/home/oz/Coding/Assignment/COL781/Assignment1/resources/objects/sphere/sphere.obj")
    {
        Matrix4f t = trans.transpose();
        model = glm::mat4( t(0,0),t(0,1),t(0,2),t(0,3),
                            t(1,0),t(1,1),t(1,2),t(1,3),
                            t(2,0),t(2,1),t(2,2),t(2,3),
                            t(3,0),t(3,1),t(3,2),t(3,3));
    }
    
    void CameraModel::Draw(Shader shader) const {
        shader.setMat4("model",model);
        cameraModel.Draw(shader);
    }
    

    // BaseModel
    void BaseModel::applyShaders(Shader shader) const {
        shader.setVec3("material.diffuse", kd);
        shader.setVec3("material.ambient", ka);
        shader.setVec3("material.specular", ks);
        shader.setFloat("material.shininess", specular_coeff);
        shader.setInt("num_texture_diffuse",0);
        shader.setInt("num_texture_specular",0);
        shader.setInt("num_texture_normal",0);
        shader.setInt("num_texture_height",0);
    }

    BaseModel::BaseModel(const Material& mat) {
        ka = glm::vec3(mat.Ka[0],mat.Ka[1],mat.Ka[2]);
        kd = glm::vec3(mat.Kd[0],mat.Kd[1],mat.Kd[2]);
        ks = glm::vec3(mat.Ks[0],mat.Ks[1],mat.Ks[2]);
        specular_coeff = mat.specular_coeff;
    }

    void BaseModel::Draw(Shader shader) const {
        applyShaders(shader);
        this->render(shader);
    }

    // Box
    Box::Box(const Point& center, const Vector3f& x, const Vector3f& y, float l,
        float b, float h, const Material& mat, const Transformation& t):
        BaseModel(mat),model(glm::mat4(1.0f)), box("/home/oz/Coding/Assignment/COL781/Assignment1/resources/objects/cube/cube.obj") {

        Vector3f xn = x.normalized();
        Vector3f zn = (xn.cross(y)).normalized();
        Vector3f yn = (zn.cross(xn)).normalized();

        model = Utils::getTransformation(t)*model;
        model = glm::translate(model,glm::vec3(center[0],center[1],center[2]));
        model = Utils::getTransformationAxis(xn,yn,zn)*model;
        model = glm::scale(model,glm::vec3(l,b,h));
    }

    void Box::render(Shader shader) const {
        shader.setMat4("model",model);
        box.Draw(shader);
    }


    // Sphere
    Sphere::Sphere(Point center, float radius, Material mat, Transformation t):
        BaseModel(mat), model(glm::mat4(1.0)), sphere("/home/oz/Coding/Assignment/COL781/Assignment1/resources/objects/sphere/sphere.obj") {
        model = Utils::getTransformation(t)*model;
        model = glm::translate(model,glm::vec3(center[0],center[1],center[2]));
        model = glm::scale(model,glm::vec3(radius));
    }

    // draws the model, and thus all its meshes
    void Sphere::render(Shader shader) const
    {
        shader.setMat4("model",model);
        sphere.Draw(shader);
    }
}
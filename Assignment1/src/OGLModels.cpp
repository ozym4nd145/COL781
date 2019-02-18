#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "OGLModels.h"

namespace ogl {

    // Lights
    Lights::Lights(const std::vector<std::pair<Point,Color>>& lights):
        lightModel("../resources/objects/sphere/sphere.obj")
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
        cameraModel("../resources/objects/camera/camera.obj")
    {
        Matrix4f t = trans.transpose();
        model = glm::transpose(glm::mat4( t(0,0),t(0,1),t(0,2),t(0,3),
                            t(1,0),t(1,1),t(1,2),t(1,3),
                            t(2,0),t(2,1),t(2,2),t(2,3),
                            t(3,0),t(3,1),t(3,2),t(3,3)));
        model = glm::translate(model,glm::vec3(0.0f,-0.5f,0.0f));
        model = glm::rotate(model,glm::radians(180.0f),glm::vec3(0.0f,0.0f,1.0f));
        model = glm::rotate(model,glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));
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
        BaseModel(mat),model(glm::mat4(1.0f)), box("../resources/objects/cube/cube.obj") {

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
        BaseModel(mat), model(glm::mat4(1.0)), sphere("../resources/objects/sphere/sphere.obj") {
        model = Utils::getTransformation(t)*model;
        model = glm::translate(model,glm::vec3(center[0],center[1],center[2]));
        model = glm::scale(model,glm::vec3(radius));
        model = glm::scale(model,glm::vec3((1/3.0))); // since blender obj is of radius 3
    }

    // draws the model, and thus all its meshes
    void Sphere::render(Shader shader) const
    {
        shader.setMat4("model",model);
        sphere.Draw(shader);
    }

    //Lines
    Lines::Lines(std::vector<std::pair<Vector3f,Vector3f>>& lines):
        model(1.0f), intensity(1.0f,0.3f,0.2f)
    {
        populate(lines);
        setupMesh();
    }
    
    void Lines::setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, pts.size()*sizeof(float), &pts[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    }

    void Lines::resetMesh() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, pts.size()*sizeof(float), &pts[0], GL_STATIC_DRAW);
    }

    void Lines::populate(std::vector<std::pair<Vector3f,Vector3f>>& lines)
    {
        pts.clear();
        for(auto line: lines) {
            pts.push_back(line.first[0]);
            pts.push_back(line.first[1]);
            pts.push_back(line.first[2]);
            pts.push_back(line.second[0]);
            pts.push_back(line.second[1]);
            pts.push_back(line.second[2]);
        }
    }

    void Lines::resetLines(std::vector<std::pair<Vector3f,Vector3f>>& lines)
    {
        populate(lines);
        resetMesh();
    }

    // draws the model, and thus all its meshes
    void Lines::Draw(Shader shader) const {
        shader.setMat4("model",model);
        shader.setVec3("intensity",intensity);
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, pts.size());

        glBindVertexArray(0);
    }
}
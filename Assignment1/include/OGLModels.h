#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/model.h>
#include <learnopengl/shader.h>

#include "defs.h"
#include "DS.h"

namespace ogl {
    class Utils {
        public:
        static glm::mat4 getTransformation(const Transformation& t) {
            Matrix3f mat = t.T_M_W.transpose();
            Vector3f trans = t.R_M_W;
            return glm::mat4(mat(0,0),mat(0,1),mat(0,2),trans[0],
                            mat(1,0),mat(1,1),mat(1,2),trans[1],
                            mat(2,0),mat(2,1),mat(2,2),trans[2],
                            0.0f,0.0f,0.0f,1.0f);
        }
        static glm::mat4 getTransformationAxis(const Vector3f& x,const Vector3f& y,const Vector3f& z) {
            return glm::mat4(x[0],y[0],z[0],0.0f,
                            x[1],y[1],z[1],0.0f,
                            x[2],y[2],z[2],0.0f,
                            0.0f,0.0f,0.0f,1.0f);
        }
        static glm::mat4 getTransformationX(const Vector3f& x) {
            return glm::mat4(x[0],0.0f,0.0f,0.0f,
                            x[1],1.0f,0.0f,0.0f,
                            x[2],0.0f,1.0f,0.0f,
                            0.0f,0.0f,0.0f,1.0f);
        }
        static glm::mat4 getTransformationY(const Vector3f& y) {
            return glm::mat4(1.0f,y[0],0.0f,0.0f,
                            0.0f,y[1],0.0f,0.0f,
                            0.0f,y[2],1.0f,0.0f,
                            0.0f,0.0f,0.0f,1.0f);
        }
        static glm::mat4 getTransformationZ(const Vector3f& z) {
            return glm::mat4(1.0f,0.0f,z[0],0.0f,
                            0.0f,1.0f,z[1],0.0f,
                            0.0f,0.0f,z[2],0.0f,
                            0.0f,0.0f,0.0f,1.0f);
        }
    };


    class BaseModel {
        private:
            glm::vec3 ka;
            glm::vec3 kd;
            glm::vec3 ks;
            float specular_coeff;

            void applyShaders(Shader shader) const {
                shader.setVec3("material.diffuse", kd);
                shader.setVec3("material.ambient", ka);
                shader.setVec3("material.specular", ks);
                shader.setFloat("material.shininess", specular_coeff);
                shader.setInt("num_texture_diffuse",0);
                shader.setInt("num_texture_specular",0);
                shader.setInt("num_texture_normal",0);
                shader.setInt("num_texture_height",0);
            }
        public:

        BaseModel(const Material& mat) {
            ka = glm::vec3(mat.Ka[0],mat.Ka[1],mat.Ka[2]);
            kd = glm::vec3(mat.Kd[0],mat.Kd[1],mat.Kd[2]);
            ks = glm::vec3(mat.Ks[0],mat.Ks[1],mat.Ks[2]);
            specular_coeff = mat.specular_coeff;
        }

        virtual void render(Shader shader) const = 0;

        void Draw(Shader shader) const {
            applyShaders(shader);
            this->render(shader);
        }
    };

    class Box: public BaseModel {
        private:
            glm::mat4 model;
            ogl::Model box;
        public:

        Box(const Point& center, const Vector3f& x, const Vector3f& y, float l,
            float b, float h, const Material& mat, const Transformation& t):
            BaseModel(mat),model(glm::mat4(1.0f)), box("../resources/objects/sphere/sphere.obj") {
            model = Utils::getTransformation(t)*model;

            Vector3f xn = x.normalized();
            Vector3f zn = (xn.cross(y)).normalized();
            Vector3f yn = (zn.cross(xn)).normalized();

            model = Utils::getTransformation(t)*model;
            model = glm::translate(model,glm::vec3(center[0],center[1],center[2]));
            model = Utils::getTransformationAxis(xn,yn,zn)*model;
            model = glm::scale(model,glm::vec3(l,b,h));
        }

        void render(Shader shader) const {
            shader.setMat4("model",model);
            box.Draw(shader);
        }
    };

    class Sphere: public BaseModel {
        private:
            glm::mat4 model;
            ogl::Model sphere;
        public:

        Sphere(Point center, float radius, Material mat, Transformation t):
            BaseModel(mat), model(glm::mat4(1.0)), sphere("../resources/objects/sphere/sphere.obj") {
            model = glm::translate(model,glm::vec3(center[0],center[1],center[2]));
            model = glm::scale(model,glm::vec3(radius));
        }

        // draws the model, and thus all its meshes
        void render(Shader shader) const
        {
            shader.setMat4("model",model);
            sphere.Draw(shader);
        }
    };
}
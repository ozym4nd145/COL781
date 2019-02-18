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
            // NOTE: Taking transpose since glm is column major!!!
            return glm::transpose(glm::mat4(mat(0,0),mat(0,1),mat(0,2),trans[0],
                            mat(1,0),mat(1,1),mat(1,2),trans[1],
                            mat(2,0),mat(2,1),mat(2,2),trans[2],
                            0.0f,0.0f,0.0f,1.0f));
        }
        static glm::mat4 getTransformationAxis(const Vector3f& x,const Vector3f& y,const Vector3f& z) {
            return glm::transpose(glm::mat4(x[0],y[0],z[0],0.0f,
                            x[1],y[1],z[1],0.0f,
                            x[2],y[2],z[2],0.0f,
                            0.0f,0.0f,0.0f,1.0f));
        }
        static glm::mat4 getTransformationX(const Vector3f& x) {
            return glm::transpose(glm::mat4(x[0],0.0f,0.0f,0.0f,
                            x[1],1.0f,0.0f,0.0f,
                            x[2],0.0f,1.0f,0.0f,
                            0.0f,0.0f,0.0f,1.0f));
        }
        static glm::mat4 getTransformationY(const Vector3f& y) {
            return glm::transpose(glm::mat4(1.0f,y[0],0.0f,0.0f,
                            0.0f,y[1],0.0f,0.0f,
                            0.0f,y[2],1.0f,0.0f,
                            0.0f,0.0f,0.0f,1.0f));
        }
        static glm::mat4 getTransformationZ(const Vector3f& z) {
            return glm::transpose(glm::mat4(1.0f,0.0f,z[0],0.0f,
                            0.0f,1.0f,z[1],0.0f,
                            0.0f,0.0f,z[2],0.0f,
                            0.0f,0.0f,0.0f,1.0f));
        }

    };
    
    class Lights {
        private:
            const float radius = 0.1;
            vector<glm::mat4> transformationMatrices;
            vector<glm::vec3> positions;
            vector<glm::vec3> intensities;
            ogl::Model lightModel;
        public:

        Lights(const std::vector<std::pair<Point,Color>>& lights);
        
        void configureLights(Shader shader) const;

        void DrawLights(Shader shader) const;
    };

    class CameraModel {
        private:
            glm::mat4 model;
            ogl::Model cameraModel;
        public:

        CameraModel(const Matrix4f& trans);
        
        void Draw(Shader shader) const;
    };
    

    class BaseModel {
        private:
            glm::vec3 ka;
            glm::vec3 kd;
            glm::vec3 ks;
            float specular_coeff;

            void applyShaders(Shader shader) const;
        public:

        BaseModel(const Material& mat);

        virtual void render(Shader shader) const = 0;

        void Draw(Shader shader) const;
    };

    class Box: public BaseModel {
        private:
            glm::mat4 model;
            ogl::Model box;
        public:

        Box(const Point& center, const Vector3f& x, const Vector3f& y, float l,
            float b, float h, const Material& mat, const Transformation& t);

        void render(Shader shader) const;
    };

    class Sphere: public BaseModel {
        private:
            glm::mat4 model;
            ogl::Model sphere;
        public:

        Sphere(Point center, float radius, Material mat, Transformation t);

        // draws the model, and thus all its meshes
        void render(Shader shader) const;
    };
}
#pragma once

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

class Track{
    public:
        Model *base_model;
        Shader *shader;

        Track(const string obj_filename, Shader* sh ): shader(sh) {
            base_model = new Model(obj_filename);
        }

        void draw_at_time(float t){

            glm::mat4 this_trans = glm::mat4(1.0f);
            shader->setMat4("model", this_trans);
            
            base_model->Draw(*shader);
            
        }
};


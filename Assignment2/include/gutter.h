#pragma once

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

class Gutter{
    public:
        Model *base_model;
        Shader *shader;
        const glm::vec3 right_trans = glm::vec3(-0.1f,0.2f,0.0f);
        const glm::vec3 left_trans = glm::vec3(-2.4 - 2*4.051050f,0.2f,0.0f);

        Gutter(const string obj_filename, Shader* sh ): shader(sh) {
            base_model = new Model(obj_filename);
        }

        void draw_at_time(float t){

            glm::mat4 right_mat = glm::translate(glm::mat4(1.0f),right_trans);
            shader->setMat4("model", right_mat);
            base_model->Draw(*shader);
            
            glm::mat4 left_mat = glm::translate(glm::mat4(1.0f),left_trans);
            shader->setMat4("model", left_mat);
            base_model->Draw(*shader);


        }
};


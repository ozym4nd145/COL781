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


            this_trans = glm::translate(this_trans,glm::vec3(0.0f,-0.9f,-5.0f));
            // this_trans = glm::rotate(this_trans,glm::radians(10.0f),glm::vec3(1.0f,0.0f,0.0f));
            this_trans = glm::scale(this_trans, glm::vec3(1.8f, 1.0f, 0.5f));

            shader->setMat4("model", this_trans);
            
            base_model->Draw(*shader);
            
        }
};


#pragma once

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

class Pin{
    public:
        Model *base_model;
        Shader *shader;
        const float time_to_hit = 3.0f;
        const float time_to_fall = 5.0f;

        Pin(const string obj_filename, Shader* sh ): shader(sh) {
            base_model = new Model(obj_filename);
        }

        void draw_at_time(float t){

            glm::mat4 this_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-0.3f,-5.5f));
            if(t >= time_to_hit){
                // ball has hit
                float exess_angle = min((85.0f/time_to_fall)*(t-time_to_hit),85.0f);
                this_trans = glm::rotate(this_trans,glm::radians(-1.0f*exess_angle),glm::vec3(0.0f,0.0f,1.0f));

            }
            this_trans = glm::rotate(this_trans,glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
            this_trans = glm::scale(this_trans, glm::vec3(0.10f, 0.10f, 0.10f));
            shader->setMat4("model", this_trans);
            
            base_model->Draw(*shader);
            
        }
};


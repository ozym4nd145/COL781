#pragma once

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

class Pin{
    public:
        Model *base_model;
        Shader *shader;
        const glm::mat4 orig_model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.05f,0.0f));
        const float max_angle_fall = 75.0f; // in degrees
        const float bottom_right_corner_dist = 0.750f;
        
        const float time_to_hit;
        const float time_to_fall;

        Pin(const string obj_filename, Shader* sh, float th, float tf): shader(sh), time_to_hit(th*0.95), time_to_fall(tf) {
            base_model = new Model(obj_filename);
        }

        void draw_at_time(float t){
            glm::mat4 this_trans = orig_model_matrix;
            if(t >= time_to_hit){
                // ball has hit
                float exess_angle = min((max_angle_fall/time_to_fall)*(t-time_to_hit),max_angle_fall);
                this_trans = glm::translate(this_trans,glm::vec3(bottom_right_corner_dist,0.0f,0.0f));
                this_trans = glm::rotate(this_trans,glm::radians(-1.0f*exess_angle),glm::vec3(0.0f,0.0f,1.0f));
                this_trans = glm::translate(this_trans,glm::vec3(-bottom_right_corner_dist,0.0f,0.0f));
            }
            shader->setMat4("model", this_trans);            
            base_model->Draw(*shader);
        }
};


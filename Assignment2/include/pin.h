#pragma once

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

class Pin{
    private:
        bool is_hit;
        float time_to_hit;
        glm::vec3 after_hit_velocity;
    public:
        Model *base_model;
        Shader *shader;
        const glm::vec3 original_pos = glm::vec3(0.0f,0.05f,-15.0f);
        const glm::mat4 orig_model_matrix = glm::translate(glm::mat4(1.0f),original_pos);
        const float max_angle_fall = 80.0f; // in degrees
        const float bottom_right_corner_dist = 0.750f;
        const float BOTTOM_RADIUS = 1.4f;
        const float MASS = 4.0f;
        
        const float time_to_fall;

        Pin(const string obj_filename, Shader* sh, float tf): shader(sh), time_to_fall(tf) {
            base_model = new Model(obj_filename);
            is_hit = false;
        }

        glm::vec3 get_center(float t){
            return original_pos;
        }

        void set_hit(glm::vec3 velocity){
            is_hit = true;
            time_to_hit = glfwGetTime();
            after_hit_velocity = velocity;
        }

        void draw_at_time(float t){
            glm::mat4 this_trans = orig_model_matrix;
            if(is_hit){
                // ball has hit
                
                t = min(t,time_to_hit+time_to_fall);
                float exess_angle = min((max_angle_fall/time_to_fall)*(t-time_to_hit),max_angle_fall);
                
                this_trans = glm::translate(this_trans,(t-time_to_hit)*after_hit_velocity);
                
                this_trans = glm::translate(this_trans,glm::vec3(bottom_right_corner_dist,0.0f,0.0f));
                this_trans = glm::rotate(this_trans,glm::radians(-1.0f*exess_angle),glm::vec3(0.0f,0.0f,1.0f));
                this_trans = glm::translate(this_trans,glm::vec3(-bottom_right_corner_dist,0.0f,0.0f));

            }
            shader->setMat4("model", this_trans);            
            base_model->Draw(*shader);
        }
};


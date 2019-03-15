#pragma once

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include "beizer.h"

const float PI = 3.14159265359;

class Ball{
    public:
        Model *base_model;
        Shader *shader;
        Beizer *bcurve;
        const float RADIUS = 0.558623325916173f;
        const float CENTER_HT = RADIUS + 0.05f;
        const float MASS = 10.0f;
        const glm::mat4 orig_model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,CENTER_HT,0.0f));
        
        const float speed;
        float time_to_hit;

        Ball(const string obj_filename, Shader* sh, Beizer* bcurve,float sp): shader(sh), bcurve(bcurve), speed(sp) {
            base_model = new Model(obj_filename);
            time_to_hit = (bcurve->length_of_curve(1e-4))/speed;
        }

        glm::vec3 get_center(float t){
            float scaled_t = min(0.99f,t/time_to_hit);
            glm::vec3 newpos = bcurve->get_pt(scaled_t);
            return newpos;
        }

        void draw_at_time(float t){

            glm::vec3 newpos = get_center(t);

            float angle_rot_abt_x = newpos.z/RADIUS;
            float angle_rot_abt_z = newpos.x/RADIUS;

            glm::mat4 this_trans = glm::translate(orig_model_matrix, newpos);

            this_trans = glm::rotate(this_trans,angle_rot_abt_x,glm::vec3(1.0f,0.0f,0.0f));
            this_trans = glm::rotate(this_trans,angle_rot_abt_z,glm::vec3(0.0f,0.0f,1.0f));
            
            shader->setMat4("model", this_trans);
            
            base_model->Draw(*shader);
            
        }

        glm::vec3 get_velocity(float t){
            float scaled_t = min(0.98f,t/time_to_hit);
            return speed * (bcurve->get_derivative(scaled_t));
        }
};


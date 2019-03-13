#pragma once

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

class Pin{
    public:
        Model *base_model;
        Shader *shader;

        Pin(const string obj_filename, Shader* sh ): shader(sh) {
            base_model = new Model(obj_filename);
        }

        void draw_at_time(float t){

            // float scaled_t = min(0.95f,t/time_to_hit);
            // glm::vec3 newpos = bcurve->get_pt(scaled_t);

            // std::cout<<scaled_t<<"  "<<newpos.x<<","<<newpos.y<<","<<newpos.z<<std::endl;

            // float angle_rot_abt_x = abs(newpos.z)/radius;
            // float angle_rot_abt_z = abs(newpos.x)/radius;
            // std::cout<<angle_rot_abt_x<<"  " << angle_rot_abt_x<<std::endl;
            glm::mat4 this_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-0.3f,-5.5f));

            this_trans = glm::rotate(this_trans,glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
            // this_trans = glm::rotate(this_trans,angle_rot_abt_z,glm::vec3(0.0f,0.0f,1.0f));
            this_trans = glm::scale(this_trans, glm::vec3(0.05f, 0.05f, 0.05f));
            shader->setMat4("model", this_trans);
            
            base_model->Draw(*shader);
            
        }
};


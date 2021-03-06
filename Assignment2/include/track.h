#pragma once

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

class Track{
    public:
        Model *base_model;
        Shader *shader;
        const float WIDTH = 2*4.051050f;
        const float HEIGHT = 2*0.10000f;
        const float LENGTH = 2*20.000000f;
        
        Track(const string obj_filename, Shader* sh ): shader(sh) {
            base_model = new Model(obj_filename);
        }

        glm::vec3 get_plane_normal(){
            return glm::vec3(0.0f,1.0f,0.0f);
        }

        void draw_at_time(float t){

            glm::mat4 this_trans = glm::mat4(1.0f);
            shader->setMat4("model", this_trans);
            
            base_model->Draw(*shader);
            
        }
};


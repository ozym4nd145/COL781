#pragma once

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

const float PI = 3.14159265359;

class Beizer{
    private:
        vector<float> coeffs;
        void init(){
            coeffs.resize(n+1);
            coeffs[0] = 1.0;
            coeffs[n] = 1.0;
            for(int i=1;i<n;i++){
                coeffs[i] = coeffs[i-1]*(n-i+1)/i;
            }
        }
    public:
        vector<glm::vec3> points;
        const int n;
        Beizer(vector<glm::vec3>& p): points(p), n(p.size()-1) {
            init();
        }
        glm::vec3 get_pt(float t){
            assert(t >= 0.0 && t<= 1.0);
            glm::vec3 pt(0.0f,0.0f,0.0f);

            float last_t_val = pow((1.0-t),n);
            float to_multiply = t/(1.0-t);

            for(int i=0;i<=n;i++){
                pt += coeffs[i]*last_t_val*points[i];
                last_t_val *= to_multiply;
            }
            return pt;
        }
};

class Ball{
    public:
        Model *base_model;
        Shader *shader;
        Beizer *bcurve;
        
        const float time_to_hit = 10.0f;
        const float radius = 1.00f;
        Ball(const string obj_filename, Shader* sh, vector<glm::vec3> bcurve_points): shader(sh) {
            base_model = new Model(obj_filename);
            bcurve = new Beizer(bcurve_points);
        }

        void draw_at_time(float t){

            float scaled_t = min(0.95f,t/time_to_hit);
            glm::vec3 newpos = bcurve->get_pt(scaled_t);

            // std::cout<<scaled_t<<"  "<<newpos.x<<","<<newpos.y<<","<<newpos.z<<std::endl;

            float angle_rot_abt_x = abs(newpos.z)/radius;
            float angle_rot_abt_z = abs(newpos.x)/radius;
            std::cout<<angle_rot_abt_x<<"  " << angle_rot_abt_x<<std::endl;
            glm::mat4 this_trans = glm::translate(glm::mat4(1.0f), newpos);

            this_trans = glm::rotate(this_trans,angle_rot_abt_x,glm::vec3(1.0f,0.0f,0.0f));
            this_trans = glm::rotate(this_trans,angle_rot_abt_z,glm::vec3(0.0f,0.0f,1.0f));
            this_trans = glm::scale(this_trans, glm::vec3(0.001f, 0.001f, 0.001f));
            shader->setMat4("model", this_trans);
            
            base_model->Draw(*shader);
            
        }
};


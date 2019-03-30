#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <cmath>
#include <iostream>

class Pins{
    private:
        const float max_angle_fall = 90.0f; // in degrees
        const float bottom_right_corner_dist = 0.750f;
        const int num_pins=10;
    public:
        vector<bool> is_hit;
        vector<float> hit_time;
        vector<glm::vec3> velocity;
        vector<glm::vec3> last_nonzero_velocity;
        vector<glm::vec3> position;
        const float mass = 4.0f;
        // const float radius = 1.4f;
        const float radius = 0.7f;
        const float deceleration = 1.0f;
        const float EPSILON = 1e-4;
        Model *base_model;
        Shader *shader;
        
        const float time_to_fall;

        Pins(const string obj_filename, Shader* sh, float tf, glm::vec3 center_pos=glm::vec3(0.0f,0.05f,-18.0f)): 
            shader(sh), time_to_fall(tf), is_hit(num_pins,false), hit_time(num_pins,-1.0f),
            velocity(num_pins,glm::vec3(0.0f)),last_nonzero_velocity(num_pins,glm::vec3(0.0f)), position(num_pins,glm::vec3(0.0f)) {
            base_model = new Model(obj_filename);
            
            /**
             *  0 1 2 3
             *   4 5 6
             *    7 8
             *     9
             */
            float center_dist = 2.1*radius;
            float height = (sqrt(3)*3*center_dist)/2.0f;
            glm::vec3 five_to_nine(0.0f,0.0f,1.0f);
            glm::vec3 zero_to_one(1.0f,0.0f,0.0f);
            glm::vec3 nine_to_eight(0.5f,0.0f,-(sqrt(3)/2.0f));
            glm::vec3 nine_to_seven(-0.5f,0.0f,-(sqrt(3)/2.0f));

            position[5] = center_pos;
            position[9] = position[5] + (five_to_nine) *(height * (2.0f/3.0f));
            position[8] = position[9] + (nine_to_eight)*(center_dist);
            position[6] = position[9] + (nine_to_eight)*(center_dist*2.0f);
            position[3] = position[9] + (nine_to_eight)*(center_dist*3.0f);
            position[7] = position[9] + (nine_to_seven)*(center_dist);
            position[4] = position[9] + (nine_to_seven)*(center_dist*2.0f);
            position[0] = position[9] + (nine_to_seven)*(center_dist*3.0f);
            position[1] = position[0] + (zero_to_one)*(center_dist);
            position[2] = position[0] + (zero_to_one)*(center_dist*2.0f);

            for(int i=0;i<position.size();i++) {
                std::cout<<i<<": "<<glm::to_string(position[i])<<std::endl;
            }
        }

        void draw_at_time(float t) {
            for(int i=0;i<position.size();i++) {
                glm::mat4 this_trans(1.0f);
                this_trans = glm::translate(this_trans,position[i]);

                if(is_hit[i]) {
                    t = min(t,hit_time[i]+time_to_fall);
                    float excess_angle = min((max_angle_fall/time_to_fall)*(t-hit_time[i]),max_angle_fall);
                    glm::vec3 vel = velocity[i];
                    float norm = glm::length(vel);
                    if(norm > EPSILON) {
                        last_nonzero_velocity[i] = velocity[i];
                    } else {
                        vel = last_nonzero_velocity[i];
                    }
                    float dist = bottom_right_corner_dist;
                    float angle = excess_angle;

                    glm::vec3 rotation_axis = glm::cross(glm::vec3(0.0f,1.0f,0.0f),vel);

                    this_trans = glm::translate(this_trans,(dist)*glm::normalize(vel));
                    this_trans = glm::rotate(this_trans,glm::radians(angle),rotation_axis);
                    this_trans = glm::translate(this_trans,(-dist)*glm::normalize(vel));
                }
                shader->setMat4("model", this_trans);            
                base_model->Draw(*shader);
            }
        }
};


// class Pin{
//     private:
//         bool is_hit;
//         float time_to_hit;
//         glm::vec3 after_hit_velocity;
//     public:
//         Model *base_model;
//         Shader *shader;
//         const glm::vec3 original_pos = glm::vec3(0.0f,0.05f,-18.0f);
//         const glm::mat4 orig_model_matrix = glm::translate(glm::mat4(1.0f),original_pos);
//         const float max_angle_fall = 180.0f; // in degrees
//         const float bottom_right_corner_dist = 0.750f;
//         const float BOTTOM_RADIUS = 1.4f;
//         const float MASS = 4.0f;
        
//         const float time_to_fall;

//         Pin(const string obj_filename, Shader* sh, float tf): shader(sh), time_to_fall(tf) {
//             base_model = new Model(obj_filename);
//             is_hit = false;
//         }

//         glm::vec3 get_center(float t){
//             return original_pos;
//         }

//         void set_hit(glm::vec3 velocity,float collisionTime){
//             is_hit = true;
//             time_to_hit = collisionTime;
//             after_hit_velocity = velocity;
//         }

//         void draw_at_time(float t){
//             glm::mat4 this_trans = orig_model_matrix;
//             if(is_hit){
//                 // ball has hit
                
//                 t = min(t,time_to_hit+time_to_fall);
//                 float excess_angle = min((max_angle_fall/time_to_fall)*(t-time_to_hit),max_angle_fall);
                
//                 this_trans = glm::translate(this_trans,(t-time_to_hit)*after_hit_velocity);
                
//                 float dist = bottom_right_corner_dist;
//                 float angle = excess_angle;

//                 glm::vec3 rotation_axis = glm::cross(glm::vec3(0.0f,1.0f,0.0f),after_hit_velocity);

//                 this_trans = glm::translate(this_trans,(dist)*glm::normalize(after_hit_velocity));
//                 this_trans = glm::rotate(this_trans,glm::radians(angle),rotation_axis);
//                 this_trans = glm::translate(this_trans,(-dist)*glm::normalize(after_hit_velocity));

//             }
//             shader->setMat4("model", this_trans);            
//             base_model->Draw(*shader);
//         }
// };


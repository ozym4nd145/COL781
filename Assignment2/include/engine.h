#pragma once
#include "pin.h"
#include "ball.h"
#include "track.h"
class Engine{
    private:
        bool did_intersect;
    public:
        Ball *ball;
        Pin *pin;
        Track *track;
        Engine(Ball* b, Pin* p, Track* t): ball(b), pin(p), track(t) {
            did_intersect = false;
        }

        bool is_intersecting(float t){
            glm::vec3 ball_center =  ball->get_center(t);
            glm::vec3 pin_center = pin->get_center(t);
            float ball_radius = ball->RADIUS;
            float pin_radius = pin->BOTTOM_RADIUS;


            glm::vec2 ball_center_2d = glm::vec2(ball_center.x,ball_center.z);
            glm::vec2 pin_center_2d = glm::vec2(pin_center.x,pin_center.z);

            bool intersection = check_circle_intersection(ball_center_2d, ball_radius, pin_center_2d, pin_radius);

            return intersection;
        }

        bool check_circle_intersection(const glm::vec2 &c1, float r1, const glm::vec2 &c2, float r2){
            float dist = glm::length(c1-c2); 
            int rad_sum = (r1 + r2);
            if (dist <= rad_sum) 
                return 1; 
            else
                return 0;
        }

        glm::vec3 get_pin_velocity(float t){
            glm::vec3 ball_velocity = ball->get_velocity(t);
            glm::vec3 pin_velocity = (pin->MASS*ball_velocity)/(ball->MASS + pin->MASS);
            return pin_velocity;
        }

        void draw_at_time(float t){
            if(!did_intersect){
                bool intersection = is_intersecting(t);
                if(intersection){
                    pin->set_hit(get_pin_velocity(t));
                    did_intersect = true;
                }
            }

            track->draw_at_time(t);
            ball->draw_at_time(t);
            pin->draw_at_time(t);
        }
};


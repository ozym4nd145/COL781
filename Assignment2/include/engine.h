#pragma once
#include "pin.h"
#include "ball.h"
#include "track.h"
#include "gutter.h"

class Engine{
    private:
        bool did_intersect;
    public:
        Ball *ball;
        Ball *ballHand;
        Pin *pin;
        Track *track;
        Gutter *gutter;
        float startBallTrack;
        Engine(Ball* b, Ball* bh, Pin* p, Track* t, Gutter* g, float startBallTrack): ball(b), ballHand(bh), pin(p), track(t), gutter(g), startBallTrack(startBallTrack) {
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
            float track_ball_time = t-startBallTrack;
            if(!did_intersect && track_ball_time > 0){
                bool intersection = is_intersecting(track_ball_time);
                if(intersection){
                    pin->set_hit(get_pin_velocity(track_ball_time),t);
                    did_intersect = true;
                }
            }

            track->draw_at_time(t);
            if(t < startBallTrack) {
                ballHand->draw_at_time(t);
            } else {
                ball->draw_at_time(t-startBallTrack);
            }
            pin->draw_at_time(t);
            gutter->draw_at_time(t);
        }
};


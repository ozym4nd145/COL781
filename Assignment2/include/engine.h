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
        Pins *pins;
        Track *track;
        Gutter *gutter;
        float startBallTrack;
        float lastSimTime;
        const float EPSILON=1e-5;
        const float elasticity = 0.95;
        Engine(Ball* b, Ball* bh, Pins* p, Track* t, Gutter* g, float startBallTrack): ball(b), ballHand(bh),
            pins(p), track(t), gutter(g), startBallTrack(startBallTrack),lastSimTime(-1.0f) {
            did_intersect = false;
        }

        void update_position(float delta_t) {
            // std::cout<<delta_t<<std::endl;
            for(int i=0;i<(pins->position).size();i++) {
                float magnitude_change = min((pins->deceleration)*delta_t,glm::length((pins->velocity)[i]));
                if(magnitude_change<EPSILON) {
                    continue;
                }
                glm::vec3 change = (magnitude_change)*glm::normalize((pins->velocity)[i]);
                (pins->velocity)[i] = (pins->velocity)[i] - change;
                (pins->position)[i] += delta_t*(pins->velocity)[i];
            }
        }

        void simluate_collision(float t,glm::vec3 ball_center,glm::vec3 ball_velocity,float ball_radius, float ball_mass) {
            const float pin_radius = pins->radius;
            const float pin_mass = pins->mass;

            // ball to pins
            for(int i=0;i<(pins->position).size();i++) {
                glm::vec3 pos = (pins->position)[i];
                glm::vec3 vel = (pins->velocity)[i];
                bool is_intersecting = check_circle_intersection(ball_center,ball_radius,pos,pin_radius);
                
                if(!is_intersecting) continue;
                glm::vec3 approach_dir = glm::normalize(pos-ball_center);
                approach_dir.y = 0; // to force in x-z plane

                float ball_comp = glm::dot(approach_dir,ball_velocity);
                float pin_comp = glm::dot(approach_dir,vel);
                float total_momentum = ball_comp*ball_mass + pin_comp*pin_mass;
                float vel_approach = ball_comp-pin_comp;
                
                if(vel_approach <= 0) continue; // this has already been hit
                float vel_sep = elasticity*vel_approach;
                float new_pin_comp = ((ball_mass*vel_sep)+total_momentum)/(ball_mass+pin_mass);

                (pins->velocity)[i] += approach_dir*(new_pin_comp-pin_comp);
                if(!(pins->is_hit)[i]) {
                    (pins->is_hit)[i] = true;
                    (pins->hit_time)[i] = t;
                }
            }

            // pins to pins
            for(int i=0;i<(pins->position).size();i++) {
                for(int j=i+1;j<(pins->position).size();j++) {
                    glm::vec3 pos1 = (pins->position)[i];
                    glm::vec3 pos2 = (pins->position)[j];
                    glm::vec3 vel1 = (pins->velocity)[i];
                    glm::vec3 vel2 = (pins->velocity)[j];
                    bool is_intersecting = check_circle_intersection(pos1,pin_radius,pos2,pin_radius);
                    
                    if(!is_intersecting) continue;
                    glm::vec3 approach_dir = glm::normalize(pos2-pos1);
                    float p1_comp = glm::dot(approach_dir,vel1);
                    float p2_comp = glm::dot(approach_dir,vel2);
                    float total_comp = p1_comp + p2_comp;
                    float vel_approach = p1_comp-p2_comp;
                    
                    if(vel_approach <= 0) continue; // this has already been hit
                    float vel_sep = elasticity*vel_approach;
                    
                    float p1_new = (total_comp-vel_sep)/2.0f;
                    float p2_new = (total_comp+vel_sep)/2.0f;

                    (pins->velocity)[i] += approach_dir*(p1_new-p1_comp);
                    (pins->velocity)[j] += approach_dir*(p2_new-p2_comp);
                    if(!(pins->is_hit)[i]) {
                        (pins->is_hit)[i] = true;
                        (pins->hit_time)[i] = t;
                    }
                    if(!(pins->is_hit)[j]) {
                        (pins->is_hit)[j] = true;
                        (pins->hit_time)[j] = t;
                    }

                    // std::cout<<"I : "<<i;
                    // std::cout<<"\nJ : "<<j;
                    // std::cout<<"\npos1: "<<glm::to_string(pos1);
                    // std::cout<<"\npos2: "<<glm::to_string(pos2);
                    // std::cout<<"\nvel1: "<<glm::to_string(vel1);
                    // std::cout<<"\nvec2: "<<glm::to_string(vel2);
                    // std::cout<<"\npin_radius: "<<pin_radius;
                    // std::cout<<"\napproach_dir: "<<glm::to_string(approach_dir);
                    // std::cout<<"\np1_comp: "<<p1_comp;
                    // std::cout<<"\np2_comp: "<<p2_comp;
                    // std::cout<<"\nvel_app: "<<vel_approach;
                    // std::cout<<"\nvel_sep: "<<vel_sep;
                    // std::cout<<"\np1_new: "<<p1_new;
                    // std::cout<<"\np2_new: "<<p2_new;
                    // std::cout<<"\nvel1_new: "<<glm::to_string((pins->velocity)[i]);
                    // std::cout<<"\nvec2_new: "<<glm::to_string((pins->velocity)[j]);
                    // std::cout<<std::endl<<std::endl;
                }
            }
        }

        bool check_circle_intersection(const glm::vec3 &c1, float r1, const glm::vec3 &c2, float r2){
            float dist = glm::length(c1-c2); 
            int rad_sum = (r1 + r2);
            if (dist <= rad_sum) 
                return 1; 
            else
                return 0;
        }


        void draw_at_time(float t){
            float track_ball_time = t-startBallTrack;
            if(lastSimTime<0.0f) {
                lastSimTime = t;
            }
            update_position(t-lastSimTime);
            if(track_ball_time > 0){
                simluate_collision(t,ball->get_center(track_ball_time),ball->get_velocity(track_ball_time),ball->RADIUS,ball->MASS);
            }

            track->draw_at_time(t);
            if(t < startBallTrack) {
                ballHand->draw_at_time(t);
            } else {
                ball->draw_at_time(t-startBallTrack);
            }
            pins->draw_at_time(t);
            gutter->draw_at_time(t);
            lastSimTime = t;
        }
};


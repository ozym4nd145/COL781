#include "wall.h"
#include <algorithm>
#include <random>
#include <vector>


Wall::Wall(int max_particles, float radius, glm::vec3 center, float ring_width)
    : psystem{max_particles}, generator(rd()),
      sphere_radius{radius},
      sphere_center{center},
      ring_width_dist{ring_width, 2 * ring_width},
      explosion_radius_dist{sphere_radius * 0.01, sphere_radius * 0.02},
      ring_width{ring_width}
       {}

void Wall::Update(float deltaTime, float currentTime,
                  const glm::vec3& cameraPosition) {

    float last_ring_phi = (rings.size() > 0 ? (*rings.rbegin())->phi_mid : -90.0);
    // float new_ring_phi_mid = -90.0f + currentTime/1000.0 + 2*ring_width_dist(generator)*deltaTime;
    float new_ring_phi_mid = last_ring_phi + 7*ring_width_dist(generator)*deltaTime;
    float new_ring_phi_spread = 2*ring_width_dist(generator);

    if(new_ring_phi_mid > 90.0f)
        made_once = true;
    if(!made_once){
        Ring* new_r = new Ring(&psystem, sphere_center, sphere_radius, new_ring_phi_mid,new_ring_phi_spread, (float)explosion_radius_dist(generator), currentTime);
        rings.push_back(new_r);
    }

    for (auto r : rings) {
        r->Update(deltaTime, currentTime, cameraPosition);
    }

    const float gravity_strength = 0.01f;

    // Simulate all particles
    for (int i = 0; i < psystem.particles.size(); i++) {
        auto p = psystem.particles[i];

        if (p->life > 0.0f) {
            p->life -= deltaTime;
            if (p->life > 0.0f && glm::length(p->color) > 0.06f) {
                glm::vec3 gravity_dir = glm::normalize(sphere_center - p->pos);
                p->speed += gravity_dir * deltaTime * gravity_strength; 
                p->pos += p->speed * (float)deltaTime;
                p->color -= deltaTime*glm::vec4(0.2f,2.0f,2.0f,0.0f);
                p->color = glm::clamp(p->color,0.0f,1.0f);
                p->cameraDist = glm::length(p->pos - cameraPosition);
            } else {
                p->cameraDist = -1.0f;
            }
        }
    }
    // for(auto el: psystem.particles){
    //     std::cout<<el;
    // }
    // std::cout<<std::endl;
}

void Wall::Draw(Shader shader) {
    // shader.setVec4("color", color);
    psystem.Draw(shader);
}

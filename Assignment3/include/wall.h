#pragma once

#include <random>
#include "particle.h"
#include "ring.h"
#include <vector>

class Wall {
    private:
        std::vector<Ring*> rings;
        ParticleSystem psystem;

        float sphere_radius;
        glm::vec3 sphere_center; // xyz co-ordinates

            
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 generator;
        std::uniform_real_distribution<double> ring_width_dist;
        std::normal_distribution<double> explosion_radius_dist;

        glm::vec4 color;

        bool made_once = false;
        float ring_width; 

    public:
        Wall(int max_particles,float radius, glm::vec3 center,float ring_width);
        void Update(float deltaTime,float currentTime,const glm::vec3& cameraPosition);
        void Draw(Shader shader);
};
#pragma once

#include <random>
#include "particle.h"
#include "explosion.h"
#include <vector>

class Ring{
    private:
        std::vector<Explosion*> explosions;
        ParticleSystem *psystem;
        glm::vec3 sphere_center;
        float sphere_radius;


        float explosion_radius;

        
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 generator;
        std::uniform_real_distribution<double> explosion_displacement_dist;

        const float density;
        float circumference; // will be calculated in constructor

    public:
        float phi_mid, phi_spread;

        Ring(ParticleSystem* psys, glm::vec3 sphere_center, float sphere_radius, float phi_mid, float phi_spread,float explosion_radius,float currentTime);
        void Update(float deltaTime,float currentTime,const glm::vec3& cameraPosition);

};
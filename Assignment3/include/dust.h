#pragma once

#include <random>
#include "particle.h"

class Dust {
    private:
        ParticleSytem psystem;
        std::default_random_engine generator;
        std::normal_distribution<double> speed_dist;
        std::uniform_real_distribution<double> direction_dist;
        std::normal_distribution<double> volume_dist;
        std::normal_distribution<double> life_dist;

        glm::vec3 center;
        glm::vec4 color;
        float timeLimit;

    public:
        Dust(int max_particles,glm::vec3 center,glm::vec4 color,float speed,float life,int volume,float timeLimit);
        void Update(float deltaTime,float currentTime,const glm::vec3& cameraPosition);
        void Draw(Shader shader);
};
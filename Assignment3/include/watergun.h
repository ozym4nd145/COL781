#pragma once

#include <random>
#include "particle.h"

class WaterGun {
    private:
        ParticleSytem psystem;
        std::default_random_engine generator;
        std::normal_distribution<double> speed_dist;
        std::normal_distribution<double> direction_dist;
        std::normal_distribution<double> volume_dist;
        std::normal_distribution<double> life_dist;
        std::normal_distribution<double> spread_dist;

        glm::vec3 center;
        glm::vec3 direction;
        glm::vec3 p1;
        glm::vec3 p2;
        glm::vec4 color;

    public:
        WaterGun(int max_particles,glm::vec3 center,glm::vec3 direction,glm::vec4 color,float speed,float spread,float life,int volume);
        void Update(float deltaTime,const glm::vec3& cameraPosition);
        void Draw(Shader shader);
};
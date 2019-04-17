#pragma once

#include <random>
#include "particle.h"

class Dust {
    private:
        ParticleSystem psystem;
        std::default_random_engine generator;
        std::normal_distribution<double> speed_dist;
        std::uniform_real_distribution<double> direction_dist;
        std::normal_distribution<double> volume_dist;
        std::normal_distribution<double> life_dist;

        glm::vec3 center;
        glm::vec4 color;
        float timeLimit;

        std::vector<glm::vec3> windPoint;
        std::vector<glm::vec3> windDirection;
        std::vector<float> windSpeed;

    public:
        Dust(int max_particles,glm::vec3 center,glm::vec4 color,const std::vector<glm::vec3>& windPoint,const std::vector<glm::vec3>& windDirection,const std::vector<float>& windSpeed,float speed,float life,int volume,float timeLimit);
        void Update(float deltaTime,float currentTime,const glm::vec3& cameraPosition);
        void Draw(Shader shader);
};
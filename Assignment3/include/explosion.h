#pragma once

#include <random>
#include "particle.h"

class Explosion {
   private:
    ParticleSystem* psystem;
    glm::vec3 normal_direction;
    float end_time;

    float init_radius;  // radius of circle in which particles will originate

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 generator;

    std::normal_distribution<double> speed_dist;
    std::normal_distribution<double> dir_dist;
    std::normal_distribution<double> life_dist;
    std::normal_distribution<double> volume_dist;
    std::uniform_real_distribution<double> uniform_0_1_dist;

    glm::vec3 p1, p2;

    bool made_once = false; 

   public:
    glm::vec3 init_point;  // xyz co-ordinates of explosion center
    Explosion(ParticleSystem* psy, glm::vec3 center, glm::vec3 direction,
              float init_radius, float speed, float life, int volume, float end_time);
    void Generate(float deltaTime, float currentTime,
                  const glm::vec3& cameraPosition);
    void Draw(Shader shader);
};
#include "explosion.h"
#include <algorithm>
#include <random>
#include <vector>

using namespace std;

Explosion::Explosion(ParticleSystem* psy, glm::vec3 center, glm::vec3 direction,
                     float init_radius, float speed, float life, int volume, float end_time)
    : psystem{psy}, generator(rd()),
      init_point{center},
      normal_direction{direction},
      init_radius{init_radius},
      speed_dist{speed, speed},
      dir_dist{0.0, 0.1f},
      volume_dist{volume,0.1*volume},
      life_dist{life, 0.1 * life},
      uniform_0_1_dist{0.0f, 1.0f},
      end_time{end_time}
{
    glm::vec3 rand_dir(1.0f, 0.0f, 0.0f);
    if (glm::dot(rand_dir, normal_direction) == 1.0f) {
        rand_dir = {0.0f, 1.0f, 0.0f};
    }

    p1 = glm::normalize(glm::cross(normal_direction, rand_dir));
    p2 = glm::normalize(glm::cross(normal_direction, p1));
}

void Explosion::Generate(float deltaTime, float currentTime,
                         const glm::vec3& cameraPosition) {

    if (currentTime > end_time)
        return;

    float screen_area = 800*600*3.14159265359f*init_radius*init_radius*glm::dot(glm::normalize(normal_direction),-1.0f*glm::normalize(init_point - cameraPosition));

    // float screen_area = 800*600*3.14159265359f*init_radius*init_radius;
    screen_area /= glm::length(init_point-cameraPosition);
    // glm::dot(glm::normalize(normal_direction),-1.0f*glm::normalize(init_point - cameraPosition));
    if(glm::length(cameraPosition - init_point) <= 1.0f){
        screen_area *= 4;
        // cout<<"Entering if"<<endl;
    }
    // screen_area=100.0f;
    int newparticles = (int)(volume_dist(generator)*std::min(0.016f,deltaTime)*screen_area);
    // std::cout<<screen_area<<" --> "<<newparticles<<std::endl;


    for (int i = 0; i < newparticles; i++) {
        Particle* p = psystem->findUnusedParticle();
        if(p==NULL)
            break;
        p->life = life_dist(generator);
        float r = init_radius * sqrt(uniform_0_1_dist(generator));
        float theta = uniform_0_1_dist(generator) * 2 * 3.14159265359f;

        p->pos = init_point + (p1 * r * (float)cos(theta)) + (p2 * r * (float)sin(theta));

        float speed = speed_dist(generator);

        glm::vec3 dir = normal_direction + (p1 * (float)dir_dist(generator)) +
                        (p2 * (float)dir_dist(generator));

        p->speed = speed * glm::normalize(dir);

        p->color = glm::vec4(1.0f,0.75f,0.1f,0.5f);

        // std::cout<<psystem->particles[particleIndex]<<std::endl;
    }
    made_once = true;
}

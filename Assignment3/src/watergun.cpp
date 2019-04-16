#include "watergun.h"
#include <algorithm>

WaterGun::WaterGun(int max_particles,glm::vec3 center,glm::vec3 direction,glm::vec4 color,float speed,float spread,float life,int volume):
    center{center},
    direction{glm::normalize(direction)},
    color{color},
    psystem{max_particles},
    speed_dist{speed,0.1*speed},
    life_dist{life,0.1*life},
    volume_dist{volume,0.1*volume},
    spread_dist{0,spread},
    direction_dist{0.0,1.0}
{
    glm::vec3 rand_dir(1.0f,0.0f,0.0f);
    if(glm::dot(rand_dir,direction)==1.0f) {
        rand_dir = {0.0f,1.0f,0.0f};
    }

    p1 = glm::normalize(glm::cross(direction,rand_dir));
    p2 = glm::normalize(glm::cross(direction,p1));
}

void WaterGun::Update(float deltaTime,const glm::vec3& cameraPosition) {
    int newparticles = (int)(volume_dist(generator)*std::min(0.016f,deltaTime));

    for(int i=0; i<newparticles; i++){
        int particleIndex = psystem.findUnusedParticle();
        psystem.particles[particleIndex].life = life_dist(generator);
        psystem.particles[particleIndex].pos = center;

        float speed = speed_dist(generator);

        glm::vec3 dir = direction+(p1*(float)spread_dist(generator))+(p2*(float)spread_dist(generator));
        
        psystem.particles[particleIndex].speed = speed*glm::normalize(dir);
    }

    // Simulate all particles
    for(int i=0; i<psystem.particles.size(); i++){
        Particle& p = psystem.particles[i];

        if(p.life > 0.0f){
            p.life -= deltaTime;
            if (p.life > 0.0f){
                p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)deltaTime * 0.5f;
                p.pos += p.speed * (float)deltaTime;
                p.cameraDist = glm::length( p.pos - cameraPosition );
            } else {
                p.cameraDist = -1.0f;
            }
        }
    }
}

void WaterGun::Draw(Shader shader) {
    shader.setVec4("color",color);
    psystem.Draw(shader);
}
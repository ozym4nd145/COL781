#include "dust.h"
#include <algorithm>

Dust::Dust(int max_particles,glm::vec3 center,glm::vec4 color,const std::vector<glm::vec3>& windPoint,const std::vector<glm::vec3>& windDirection,const std::vector<float>& windSpeed,float speed,float life,int volume,float timeLimit):
    center{center},
    color{color},
    psystem{max_particles},
    windPoint{windPoint.begin(),windPoint.end()},
    windDirection{windDirection.begin(),windDirection.end()},
    windSpeed{windSpeed.begin(),windSpeed.end()},
    speed_dist{speed,0.5*speed},
    life_dist{life,0.1*life},
    volume_dist{volume,0.1*volume},
    direction_dist{-1.0f,1.0f},
    timeLimit{timeLimit}
{
    for(int i=0;i<windDirection.size();i++) {
        this->windDirection[i] = glm::normalize(windDirection[i]);
    }
    assert(windDirection.size()==windPoint.size());
    assert(windSpeed.size()==windPoint.size());
}

void Dust::Update(float deltaTime,float currentTime, const glm::vec3& cameraPosition) {
    // if(currentTime < timeLimit) {
        int newparticles = (int)(volume_dist(generator)*std::min(0.016f,deltaTime));

        for(int i=0; i<newparticles; i++){
            int particleIndex = psystem.findUnusedParticle();
            psystem.particles[particleIndex].life = life_dist(generator);
            psystem.particles[particleIndex].pos = center;

            float speed = speed_dist(generator);

            glm::vec3 dir = glm::vec3((float)direction_dist(generator),(float)direction_dist(generator),(float)direction_dist(generator));
            
            psystem.particles[particleIndex].speed = speed*glm::normalize(dir);
        }
    // }

    // Simulate all particles
    for(int i=0; i<psystem.particles.size(); i++){
        Particle& p = psystem.particles[i];

        if(p.life > 0.0f){
            p.life -= deltaTime;
            if (p.life > 0.0f){
                p.speed -= p.speed*((float)deltaTime)*0.05f;
                p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)deltaTime * 0.05f; // gravity

                for(int j=0;j<windPoint.size();j++) {
                    glm::vec3 point = windPoint[j];
                    glm::vec3 dir = windDirection[j];
                    float ws = windSpeed[j];
                    float dist = glm::length(glm::cross((p.pos - point),dir));
                    float effective_ws = ws/(1+0.1*(dist*dist));
                    p.speed += dir * (float)deltaTime * effective_ws; // wind
                }

                p.pos += p.speed * (float)deltaTime;
                p.cameraDist = glm::length( p.pos - cameraPosition );
            } else {
                p.cameraDist = -1.0f;
            }
        }
    }
}

void Dust::Draw(Shader shader) {
    shader.setVec4("color",color);
    psystem.Draw(shader);
}
#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/string_cast.hpp>

#include "particle.h"

std::ostream &operator<<(std::ostream &os, Particle const &m) { 
    return os << "Pos: "<<glm::to_string(m.pos)<<std::endl \
              <<"\tColor: "<<glm::to_string(m.color)<<std::endl \
              <<"\tSpeed: "<<glm::to_string(m.speed)<<std::endl \
              <<"\tlife: "<<m.life<<"\tcamDist: "<<m.cameraDist<<std::endl;
}

bool compare(const Particle* p1, const Particle *p2){
    return p1->cameraDist > p2->cameraDist;
}

ParticleSystem::ParticleSystem(const std::vector<glm::vec3>& vertices, int max_particles):
    vertices{vertices.begin(),vertices.end()},
    position{max_particles},
    color{max_particles},
    max_particles{max_particles}
{
    setupSystem();
}

ParticleSystem::ParticleSystem(int max_particles):
    position{max_particles},
    color{max_particles},
    max_particles{max_particles}
{
    vertices = {
        {0.0f, 0.0f, 0.0f}
    };
    setupSystem();
}

void ParticleSystem::Draw(Shader shader) {
    int num_particles = 0;
    for(auto& particle: particles) {
        if(particle->life < 0) {
            particle->cameraDist = -1.0f;
            unusedParticles.insert(particle);
        } else if (particle->cameraDist > 0) {
            num_particles++;
        }
    }
    std::sort(particles.begin(), particles.end(),compare);

    for(int i=0;i<num_particles;i++) {
        position[i] = particles[i]->pos;
        color[i] = 255.0f*particles[i]->color;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    glBufferData(GL_ARRAY_BUFFER, position.size() * 3 * sizeof(float), NULL, GL_STREAM_DRAW); // Buffer orphaning
    glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles * sizeof(float) * 3, &position[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glBufferData(GL_ARRAY_BUFFER, color.size() * 3 * sizeof(uint8_t), NULL, GL_STREAM_DRAW); // Buffer orphaning
    glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles * sizeof(uint8_t) * 3, &color[0]);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_POINTS, 0, vertices.size(), num_particles);
    glBindVertexArray(0);
}

void ParticleSystem::setupSystem() {
    particles.resize(max_particles,NULL);
    for(int i=0;i<max_particles;i++){
        particles[i] = new Particle();
        unusedParticles.insert(particles[i]);
    }
    
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_vertex);
    glGenBuffers(1, &VBO_pos);
    glGenBuffers(1, &VBO_color);

    glBindVertexArray(VAO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
    // glBufferData(GL_ARRAY_BUFFER, vertices.size()*3*sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    glBufferData(GL_ARRAY_BUFFER, position.size()*3*sizeof(float), NULL, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glBufferData(GL_ARRAY_BUFFER, color.size()*3*sizeof(uint8_t), NULL, GL_STREAM_DRAW);


    // glEnableVertexAttribArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_TRUE, 3*sizeof(uint8_t), (void*)0);

    // glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);


    glBindVertexArray(0);
}

Particle* ParticleSystem::findUnusedParticle() {
    if(unusedParticles.size() == 0) {
        int idx = rand()%(int)(particles.size());
        return particles[idx];
    }

    auto itr = unusedParticles.begin();
    Particle *to_return = *itr;
    unusedParticles.erase(itr);
    return to_return;

    // for(int i=lastUsedParticle;i<particles.size();i++) {
    //     if(particles[i].life <= 0) {
    //         lastUsedParticle = i;
    //         return i;
    //     }
    // }
    // for(int i=0;i<lastUsedParticle;i++) {
    //     if(particles[i].life <= 0) {
    //         lastUsedParticle = i;
    //         return i;
    //     }
    // }
    // lastUsedParticle++;
    // lastUsedParticle %= particles.size();
    // return lastUsedParticle;
}
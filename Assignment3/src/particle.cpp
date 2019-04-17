#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/string_cast.hpp>

#include "particle.h"

std::ostream &operator<<(std::ostream &os, Particle const &m) { 
    return os << "Pos: "<<glm::to_string(m.pos)<<std::endl \
              <<"\tColor: "<<glm::to_string(m.color)<<std::endl \
              <<"\tSpeed: "<<glm::to_string(m.speed)<<std::endl \
              <<"\tlife: "<<m.life<<"\tcamDist: "<<m.cameraDist<<std::endl;
}

ParticleSystem::ParticleSystem(const std::vector<glm::vec3>& vertices, int max_particles):
    vertices{vertices.begin(),vertices.end()},
    particles{max_particles},
    position{max_particles},
    color{max_particles},
    lastUsedParticle{0}
{
    setupSystem();
}

ParticleSystem::ParticleSystem(int max_particles):
    particles{max_particles},
    position{max_particles},
    color{max_particles},
    lastUsedParticle{0}
{
    vertices = {
        {0.0f, 0.0f, 0.0f}
    };
    setupSystem();
}

void ParticleSystem::Draw(Shader shader) {
    int num_particles = 0;
    for(auto& particle: particles) {
        if(particle.life < 0) {
            particle.cameraDist = -1.0f;
        } else if (particle.cameraDist > 0) {
            num_particles++;
        }
    }
    std::sort(particles.begin(), particles.end());

    for(int i=0;i<num_particles;i++) {
        position[i] = particles[i].pos;
        // color[i] = particles[i].color;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    glBufferData(GL_ARRAY_BUFFER, position.size() * 3 * sizeof(float), NULL, GL_STREAM_DRAW); // Buffer orphaning
    glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles * sizeof(float) * 3, &position[0]);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    // glBufferData(GL_ARRAY_BUFFER, color.size() * 3 * sizeof(uint8_t), NULL, GL_STREAM_DRAW); // Buffer orphaning
    // glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles * sizeof(uint8_t) * 3, &color[0]);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_POINTS, 0, vertices.size(), num_particles);
    glBindVertexArray(0);
}

void ParticleSystem::setupSystem() {
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

    // glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    // glBufferData(GL_ARRAY_BUFFER, color.size()*3*sizeof(uint8_t), NULL, GL_STREAM_DRAW);


    // glEnableVertexAttribArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    // glEnableVertexAttribArray(2);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    // glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_TRUE, 3*sizeof(uint8_t), (void*)0);

    // glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    // glVertexAttribDivisor(2, 1);


    glBindVertexArray(0);
}

int ParticleSystem::findUnusedParticle() {
    for(int i=lastUsedParticle;i<particles.size();i++) {
        if(particles[i].life <= 0) {
            lastUsedParticle = i;
            return i;
        }
    }
    for(int i=0;i<lastUsedParticle;i++) {
        if(particles[i].life <= 0) {
            lastUsedParticle = i;
            return i;
        }
    }
    lastUsedParticle++;
    lastUsedParticle %= particles.size();
    return lastUsedParticle;
}
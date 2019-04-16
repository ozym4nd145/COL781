#pragma once

#define GLM_ENABLE_EXPERIMENTAL 1

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <learnopengl/shader.h>

#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <vector>
#include <random>

struct Particle {
    glm::vec3 pos;
	glm::vec3 speed;
    glm::u8vec3 color;
    float life;
    float cameraDist;


    Particle() {
        life = -1.0f;
        cameraDist = -1.0f;
        color = {255,15,100};
        pos = {0,0,0};
        speed = {0,0,0};
    }
    
    bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameraDist > that.cameraDist;
    }


};


std::ostream &operator<<(std::ostream &os, Particle const &m) { 
    return os << "Pos: "<<glm::to_string(m.pos)<<std::endl<<"\tColor: "<<glm::to_string(m.color)<<std::endl<<"\tlife: "<<m.life<<"\tcamDist: "<<m.cameraDist<<std::endl;
}

class ParticleSytem {
    private:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> position;
        std::vector<glm::u8vec3> color;
        unsigned int VAO;
        unsigned int VBO_vertex, VBO_pos, VBO_color;
        int lastUsedParticle;

    public:
        std::vector<Particle> particles;
        ParticleSytem(const std::vector<glm::vec3>& vertices, int max_particles):
            vertices{vertices.begin(),vertices.end()},
            particles{max_particles},
            position{max_particles},
            color{max_particles},
            lastUsedParticle{0}
        {
            setupSystem();
        }

        ParticleSytem(int max_particles):
            particles{max_particles},
            position{max_particles},
            color{max_particles},
            lastUsedParticle{0}
        {
            vertices = {
                {-0.5f, -0.5f, 0.0f},
                // {-0.5f, -0.5f, 0.0f},
                // {0.5f, -0.5f, 0.0f},
                // {-0.5f, 0.5f, 0.0f},
                // {0.5f, 0.5f, 0.0f},
            };
            setupSystem();
        }

        void Draw(Shader shader) {
            int num_particles = 0;
            for(auto& particle: particles) {
                if(particle.life < 0) {
                    particle.cameraDist = -1.0f;
                } else if (particle.cameraDist > 0) {
                    num_particles++;
                }
            }
            std::sort(particles.begin(), particles.end());

            // std::cout<<"Total draw particles: "<<num_particles<<std::endl;


            for(int i=0;i<num_particles;i++) {
                position[i] = particles[i].pos;
                color[i] = particles[i].color;
                // std::cout<<"i: "<<i<<" | particles: "<<particles[i]<<std::endl;

                // color[i] = glm::uvec3(255,0,0);
            }

            glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
            glBufferData(GL_ARRAY_BUFFER, position.size() * 3 * sizeof(float), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
            glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles * sizeof(float) * 3, &position[0]);

            glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
            glBufferData(GL_ARRAY_BUFFER, color.size() * 3 * sizeof(uint8_t), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
            glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles * sizeof(uint8_t) * 3, &color[0]);

            // draw mesh
            glBindVertexArray(VAO);
            glDrawArraysInstanced(GL_POINTS, 0, vertices.size(), num_particles);
            glBindVertexArray(0);
        }

        void setupSystem() {
            // create buffers/arrays
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO_vertex);
            glGenBuffers(1, &VBO_pos);
            glGenBuffers(1, &VBO_color);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
            glBufferData(GL_ARRAY_BUFFER, vertices.size()*3*sizeof(float), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
            glBufferData(GL_ARRAY_BUFFER, position.size()*3*sizeof(float), NULL, GL_STREAM_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
            glBufferData(GL_ARRAY_BUFFER, color.size()*3*sizeof(uint8_t), NULL, GL_STREAM_DRAW);


            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
            glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_TRUE, 3*sizeof(uint8_t), (void*)0);

            glVertexAttribDivisor(0, 0);
            glVertexAttribDivisor(1, 1);
            glVertexAttribDivisor(2, 1);


            glBindVertexArray(0);
        }

        int findUnusedParticle() {
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
};


class QuadParticleSystem {
    private:
        ParticleSytem psystem;
        std::default_random_engine generator;
        std::normal_distribution<double> speed_dist;
        std::normal_distribution<double> direction_dist;


    public:
        QuadParticleSystem(int max_particles):
            psystem{max_particles},
            speed_dist{8.0,2.0},
            direction_dist{0.0,1.0}
        {
        }

        void Update(float deltaTime,const glm::vec3& cameraPosition) {
            int newparticles = (int)(deltaTime*10000.0);
            

            if (newparticles > (int)(0.016f*10000.0))
                newparticles = (int)(0.016f*10000.0);
            
            for(int i=0; i<newparticles; i++){
                int particleIndex = psystem.findUnusedParticle();
                psystem.particles[particleIndex].life = 2.0f; // This particle will live 5 seconds.
                psystem.particles[particleIndex].pos = glm::vec3(0,0,-20.0f);

                float spread = 1.0f;

                float speed = speed_dist(generator);

                glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
                glm::vec3 randomdir = glm::vec3(
                    direction_dist(generator),
                    direction_dist(generator),
                    direction_dist(generator)
                );
                
                psystem.particles[particleIndex].speed = speed*glm::normalize(maindir + randomdir*spread);


                // Very bad way to generate a random color
                psystem.particles[particleIndex].color = {rand() % 256,rand() % 256,rand() % 256};


            }
            // Simulate all particles
            for(int i=0; i<psystem.particles.size(); i++){
                Particle& p = psystem.particles[i];

                if(p.life > 0.0f){
                    // Decrease life
                    p.life -= deltaTime;
                    if (p.life > 0.0f){
                        // Simulate simple physics : gravity only, no collisions

                        p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)deltaTime * 0.5f;
                        p.pos += p.speed * (float)deltaTime;
                        p.cameraDist = glm::length( p.pos - cameraPosition );

                    } else {
                        // Particles that just died will be put at the end of the buffer in SortParticles();
                        p.cameraDist = -1.0f;
                    }
                }
            }
        }

        void Draw(Shader shader) {
            psystem.Draw(shader);
        }
};
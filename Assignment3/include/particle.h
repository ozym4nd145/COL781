#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <learnopengl/shader.h>

#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_set>

struct Particle {
    glm::vec3 pos;
	glm::vec3 speed;
    glm::vec3 color;
    float life;
    float cameraDist;


    Particle() {
        life = -1.0f;
        cameraDist = -1.0f;
        color = {1.0f,0.75f,0.1f};
        pos = {0,0,0};
        speed = {0,0,0};
    }
    
    // bool operator<(const Particle& that) const {
	// 	// Sort in reverse order : far particles drawn first.
	// 	return this->cameraDist > that.cameraDist;
    // }
};


std::ostream &operator<<(std::ostream &os, Particle const &m);

class ParticleSystem {
    private:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> position;
        std::vector<glm::u8vec3> color;
        unsigned int VAO;
        unsigned int VBO_vertex, VBO_pos, VBO_color;
        int max_particles;
        
        std::unordered_set<Particle*> unusedParticles;

        void setupSystem();

    public:
        std::vector<Particle*> particles;
        ParticleSystem(const std::vector<glm::vec3>& vertices, int max_particles);

        ParticleSystem(int max_particles);

        void Draw(Shader shader);

        Particle* findUnusedParticle();
};
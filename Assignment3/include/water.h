#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/mesh.h>

#include <string>
#include <vector>


class Water {
    private:
        glm::mat4 modelTransformation;
        Mesh* mesh;
        float size;
	    int vertexCount;
        std::vector<Texture> diffuseTexture;

        inline int get(int i,int j) {return i*vertexCount+j;}

        void setupTerrain();
    public:

        Water(glm::vec3 position,float size=800,int vertexCount=128,std::vector<std::string> diffuseTexture={});
        void Draw(Shader shader);
};
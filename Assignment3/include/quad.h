#pragma once

#include <vector>
#include <glad/glad.h> // holds all OpenGL type declarations
#include <learnopengl/shader.h>

class Quad {
    private:
        unsigned int VAO;
        unsigned int VBO;
        void setupQuad();
    public:
        Quad();
        void Draw(Shader shader,const std::vector<unsigned int>& textureIds);        
};
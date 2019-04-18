#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/mesh.h>

#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <vector>


class Terrain {
    private:
        glm::mat4 modelTransformation;
        Mesh* mesh;
        float size;
	    int vertexCount;
        std::vector<Texture> diffuseTexture;
        const float max_pixel_colour=256*256*256;
        const float max_height = 80;
        void setupTerrain(std::string& heightMapPath);
    public:
        Terrain(int x, int z,float size=800,int vertexCount=128,std::vector<std::string> diffuseTexture={},std::string heightMapPath="");
        void Draw(Shader shader);
        float getHeight(int x, int z, const stb::image& image);
};
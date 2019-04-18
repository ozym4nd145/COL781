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
#include <random>


class Terrain {
    private:
        glm::mat4 modelTransformation;
        Mesh* mesh;
        float size;
	    int vertexCount;
        std::vector<Texture> diffuseTexture;
        const float max_pixel_colour=256*256*256;
        const float max_height = 80;
        const float heightScale;

        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 generator;
        std::uniform_real_distribution<float> rand_dist;

        void setupTerrain(std::string& heightMapPath);
        void generateTerrain(std::vector<Vertex>& vertices,int level,float scale);
        inline int get(int i,int j) {return i*vertexCount+j;}
    public:
        float min_terrain_height;
        float max_terrain_height;
        float average_terrain_height;

        Terrain(int x, int z,float size=800,int vertexCount=128,float heightScale=10,std::vector<std::string> diffuseTexture={},std::string heightMapPath="");
        void Draw(Shader shader);
        float getHeight(int x, int z, const stb::image& image);
};
#include "terrain.h"
#include <learnopengl/model.h>
#include <stb_image.hpp>

Terrain::Terrain(int x, int z,float size,int vertexCount,std::vector<std::string> diffuseTexture,std::string heightMapPath):
    modelTransformation{glm::mat4(1.0f)},
    mesh{nullptr},
    size{size},
    vertexCount{vertexCount}
{
    assert(vertexCount > 1);
    modelTransformation = glm::translate(modelTransformation,{x,0,z});
    for(auto& texturePath: diffuseTexture) {
        this->diffuseTexture.push_back(Texture{Model::TextureFromFile(texturePath.c_str(),"."),"texture_diffuse",texturePath});
    }
    
    setupTerrain(heightMapPath);
}

void Terrain::Draw(Shader shader) {
    shader.setMat4("model",modelTransformation);
    mesh->Draw(shader);
}


void Terrain::setupTerrain(std::string& heightMapPath) {
    stb::image heightMap{heightMapPath, 4};

    int numVertices = vertexCount*vertexCount;
    int numIndices = 6*(vertexCount - 1)*(vertexCount - 1);
    vector<Vertex> vertices(numVertices);
    vector<unsigned int> indices(numIndices);

    for(int idx = 0, i=0;i<vertexCount;i++) {
        for(int j=0;j<vertexCount;j++,idx++) {
            float fracX = (float)i/((float)(vertexCount-1));
            float fracZ = (float)j/((float)(vertexCount-1));
            float height = getHeight(i, j, heightMap);

            vertices[idx].Position = glm::vec3(fracX*size,height,fracZ*size);
            vertices[idx].Normal = glm::vec3(0.0f,1.0f,0.0f);
            vertices[idx].TexCoords = glm::vec2(fracX,fracZ);
            vertices[idx].Tangent = glm::vec3(1.0f,0.0f,0.0f);
            vertices[idx].Bitangent = glm::vec3(0.0f,0.0f,1.0f);
        }
    }

    for(int idx = 0, x=0;x<vertexCount-1;x++) {
        for(int z=0;z<vertexCount-1;z++) {
            int topLeft = (z*vertexCount)+x;
            int topRight = topLeft+1;
            int bottomLeft = topLeft+vertexCount;
            int bottomRight = bottomLeft+1;

            indices[idx++] = topLeft;
            indices[idx++] = topRight;
            indices[idx++] = bottomRight;
            indices[idx++] = bottomRight;
            indices[idx++] = bottomLeft;
            indices[idx++] = topLeft;
        }
    }

    mesh = new Mesh(vertices,indices,diffuseTexture);
}


float Terrain::getHeight(int x, int z, const stb::image& image)
{
    if (x < 0 || x > image.height() || z < 0 || z > image.height()) {
        return 0;
    }

    uint32_t val = image.get_rgb(x, z);

    double height = val;
    height /= (max_pixel_colour/2);
    height -= 1.0;
    height *= max_height;
    //std::cout << height << std::endl;

    return height;
}

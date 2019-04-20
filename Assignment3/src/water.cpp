#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/string_cast.hpp>

#include "water.h"
#include <learnopengl/model.h>

#include<iostream>
using namespace std;


// class Water {
//     private:
//         glm::mat4 modelTransformation;
//         Mesh* mesh;
//         float size;
// 	    int vertexCount;
//         std::vector<Texture> diffuseTexture;

//         inline int get(int i,int j) {return i*vertexCount+j;}
//     public:

// };

Water::Water(glm::vec3 position,float size,int vertexCount,std::vector<std::string> diffuseTexture):
    modelTransformation{glm::translate(glm::mat4{1.0f},position)},
    mesh{nullptr},
    size{size},
    vertexCount{exp2((int(log(vertexCount-1)/log(2))+1)) + 1} // round to power of 2^x + 1
{
    assert(vertexCount > 1);

    cout<<"position: "<<glm::to_string(position)<<endl;

    for(auto& texturePath: diffuseTexture) {
        this->diffuseTexture.push_back(Texture{Model::TextureFromFile(texturePath.c_str(),"."),"texture_diffuse",texturePath});
    }
    
    setupTerrain();
}

void Water::setupTerrain() {
    int numVertices = vertexCount*vertexCount;
    int numIndices = 6*(vertexCount - 1)*(vertexCount - 1);

    vector<Vertex> vertices(numVertices);
    vector<unsigned int> indices(numIndices);

    for(int idx = 0, i=0;i<vertexCount;i++) {
        for(int j=0;j<vertexCount;j++,idx++) {
            float fracX = (float)i/((float)(vertexCount-1));
            float fracZ = (float)j/((float)(vertexCount-1));

            vertices[idx].Position = glm::vec3(fracX*size,0,fracZ*size);
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

void Water::Draw(Shader shader) {
    shader.setMat4("model",modelTransformation);
    mesh->Draw(shader);
}
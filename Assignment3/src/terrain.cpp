#include "terrain.h"
#include <learnopengl/model.h>
#include <stb_image.hpp>

#include<iostream>
using namespace std;

Terrain::Terrain(int x, int z,float size,int vertexCount,float heightScale,std::vector<std::string> diffuseTexture,std::string heightMapPath):
    modelTransformation{glm::mat4(1.0f)},
    mesh{nullptr},
    size{size},
    vertexCount{exp2((int(log(vertexCount-1)/log(2))+1)) + 1}, // round to power of 2^x + 1
    heightScale{heightScale},
    generator{rd()},
    rand_dist{0.0,1.0},
    min_terrain_height{0.0f},
    max_terrain_height{0.0f},
    average_terrain_height{0.0f}
{
    assert(vertexCount > 1);
    cout<<"Vertex Count: "<<this->vertexCount<<endl;

    modelTransformation = glm::translate(modelTransformation,{x,0,z});
    for(auto& texturePath: diffuseTexture) {
        this->diffuseTexture.push_back(Texture{Model::TextureFromFile(texturePath.c_str(),"."),"texture_diffuse",texturePath});
    }
    
    setupTerrain(heightMapPath);
}

void Terrain::Draw(Shader shader) {
    // for(int i=-2;i<3;i++) {
        // for(int j=-2;j<3;j++) {
            // shader.setMat4("model",glm::translate(modelTransformation,{i*(size-10),0,j*(size-10)}));
            // mesh->Draw(shader);
        // }
    // }
    shader.setMat4("model",modelTransformation);
    mesh->Draw(shader);
}



void Terrain::generateTerrain(std::vector<Vertex>& vertices,int level,float scale,std::vector<int>& visit) {

    if(level <= 1) return;

    // diamonds
    for(int i=0;i<vertexCount-1;i+=level) {
        for(int j=0;j<vertexCount-1;j+=level) {
            float topL = vertices[get(i,j)].Position[1];
            float topR = vertices[get(i+level,j)].Position[1];
            float bottomL = vertices[get(i,j+level)].Position[1];
            float bottomR = vertices[get(i+level,j+level)].Position[1];
            float mid = (topL+topR+bottomL+bottomR)/4 + scale*rand_dist(generator);
            vertices[get(i+level/2,j+level/2)].Position[1] = mid;
            
            visit[get(i+level/2,j+level/2)]++;
        }
    }

    // corner cases
    for(int i=0;i<vertexCount-1;i+=level) {
        float top = vertices[get(i,0)].Position[1];
        float bottom = vertices[get(i+level,0)].Position[1];
        float rSide = vertices[get(i+level/2,level/2)].Position[1];
        float lSide = vertices[get(i+level/2,(vertexCount-1)-level/2)].Position[1];
        float mid = (top+bottom+rSide+lSide)/4 + scale*rand_dist(generator);
        vertices[get(i+level/2,0)].Position[1] = mid;
        visit[get(i+level/2,0)]++;

        vertices[get(i+level/2,(vertexCount-1))].Position[1] = mid;
        visit[get(i+level/2,(vertexCount-1))]++;
    }
    for(int j=0;j<vertexCount-1;j+=level) {
        float left = vertices[get(0,j)].Position[1];
        float right = vertices[get(0,j+level)].Position[1];
        float tSide = vertices[get(level/2,j+level/2)].Position[1];
        float bSide = vertices[get((vertexCount-1)-level/2,j+level/2)].Position[1];
        float mid = (tSide+bSide+left+right)/4 + scale*rand_dist(generator);
        vertices[get(0,j+level/2)].Position[1] = mid;
        visit[get(0,j+level/2)]++;

        vertices[get((vertexCount-1),j+level/2)].Position[1] = mid;
        visit[get((vertexCount-1),j+level/2)]++;
    }

    // square
    for(int i=level;i<vertexCount;i+=level) {
        for(int j=level;j<vertexCount;j+=level) {
            float cur = vertices[get(i,j)].Position[1];
            float top = vertices[get(i-level,j)].Position[1];
            float left = vertices[get(i,j-level)].Position[1];
            float topL = vertices[get(i-level/2,j-level/2)].Position[1];
            if((j+(level/2))<vertexCount) {
                float topR = vertices[get(i-level/2,j+level/2)].Position[1];
                float topMid = (cur+top+topR+topL)/4 + scale*rand_dist(generator);
                vertices[get(i-level/2,j)].Position[1] = topMid;
                visit[get(i-level/2,j)]++;
            }
            if((i+(level/2))<vertexCount) {
                float bottomL = vertices[get(i+level/2,j-level/2)].Position[1];
                float leftMid = (cur+left+bottomL+topL)/4 + scale*rand_dist(generator);
                vertices[get(i,j-level/2)].Position[1] = leftMid;
                visit[get(i,j-level/2)]++;
            }
        }
    }

    return generateTerrain(vertices,level/2,scale/2,visit);
}


void Terrain::setupTerrain(std::string& heightMapPath) {
    stb::image heightMap{heightMapPath, 4};

    cout<<"Vertex Count: "<<vertexCount<<endl;

    int numVertices = vertexCount*vertexCount;
    int numIndices = 6*(vertexCount - 1)*(vertexCount - 1);

    cout<<"num vertex: "<<numVertices<<endl;
    cout<<"num index: "<<numIndices<<endl;

    vector<Vertex> vertices(numVertices);
    vector<unsigned int> indices(numIndices);

    cout<<"vertex size: "<<vertices.size()<<endl;
    cout<<"indices size: "<<indices.size()<<endl;

    for(int idx = 0, i=0;i<vertexCount;i++) {
        for(int j=0;j<vertexCount;j++,idx++) {
            float fracX = (float)i/((float)(vertexCount-1));
            float fracZ = (float)j/((float)(vertexCount-1));
            // float height = getHeight(i, j, heightMap);
            float height = 0;

            vertices[idx].Position = glm::vec3(fracX*size,height,fracZ*size);
            vertices[idx].Normal = glm::vec3(0.0f,1.0f,0.0f);
            vertices[idx].TexCoords = glm::vec2(fracX,fracZ);
            vertices[idx].Tangent = glm::vec3(1.0f,0.0f,0.0f);
            vertices[idx].Bitangent = glm::vec3(0.0f,0.0f,1.0f);
        }
    }

    cout<<"vertex size: "<<vertices.size()<<endl;
    cout<<"indices size: "<<indices.size()<<endl;

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

    std::vector<int> visit(vertices.size(),0);
    generateTerrain(vertices,vertexCount-1,heightScale,visit);
    
    int count=0;
    for(int i=0;i<vertexCount;i++) {
        for(int j=0;j<vertexCount;j++) {
            vertices[get(i,j)].Normal = calculateNormal(i,j,vertices);
            float height = getHeight(i,j,vertices);
            min_terrain_height = std::min(height,min_terrain_height);
            max_terrain_height = std::max(height,max_terrain_height);
            average_terrain_height += height;
            count++;

            // cout<<visit[get(i,j)]<<",";
        }
        // cout<<endl;
    }
    // cout<<endl;
    average_terrain_height /= count;

    mesh = new Mesh(vertices,indices,diffuseTexture);
}

glm::vec3 Terrain::calculateNormal(int i, int j,const std::vector<Vertex>& vertices) {
    float left = getHeight(i - 1, j, vertices);
    float right = getHeight(i + 1, j, vertices);
    float bottom = getHeight(i, j - 1, vertices);
    float top = getHeight(i, j + 1, vertices);

    glm::vec3 normal{left - right, 2.0f, bottom - top};
    return glm::normalize(normal);
}

float Terrain::getHeight(int x, int z, const std::vector<Vertex>& vertices)
{
    if (x < 0 || x > vertexCount || z < 0 || z > vertexCount) {
        return 0;
    }
    return vertices[get(x, z)].Position[1];
}

float Terrain::getHeight(int x, int z, const stb::image& image)
{
    if (x < 0 || x > image.width() || z < 0 || z > image.height()) {
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

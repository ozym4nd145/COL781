#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <learnopengl/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // number of joints
    glm::ivec1 NumJoints;
    // jointIndices
    glm::ivec3 JointIndices;
    // jointWeights
    glm::vec3 JointWeights;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};


class AnimatedMesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;

    /*  Functions  */
    // constructor
    AnimatedMesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(Shader shader) 
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 0;
        unsigned int specularNr = 0;
        unsigned int normalNr   = 0;
        unsigned int heightNr   = 0;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if(name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if(name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
             else if(name == "texture_height")
			    number = std::to_string(heightNr++); // transfer unsigned int to stream

													 // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name +"["+ number+"]").c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        glUniform1i(glGetUniformLocation(shader.ID, ("num_texture_diffuse")), diffuseNr);
        glUniform1i(glGetUniformLocation(shader.ID, ("num_texture_specular")), specularNr);
        glUniform1i(glGetUniformLocation(shader.ID, ("num_texture_normal")), normalNr);
        glUniform1i(glGetUniformLocation(shader.ID, ("num_texture_height")), heightNr);
                
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    /*  Render data  */
    unsigned int VBO, EBO;

    /*  Functions    */
    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // number of joints
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, NumJoints));
        // joint indices
        glEnableVertexAttribArray(4);
        glVertexAttribIPointer(4, 3, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, JointIndices));
        // joint weights
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, JointWeights));

        glBindVertexArray(0);
    }
};

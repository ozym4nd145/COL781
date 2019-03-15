#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AnimatedMesh.h"


using namespace std;

namespace anim {

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Joint {
    public:
        int id;
        std::string name;
        std::vector<Joint*> children;
        glm::mat4 animatedTransform;
        glm::mat4 offsetTransform;
        glm::mat4 localBindTransform;
        glm::mat4 invBindTransform;

        Joint(int id,const std::string& name):
            localBindTransform{1.0f},invBindTransform{1.0f},id{id}, name{name}, animatedTransform{1.0f},offsetTransform{1.0f} {
            cout<<"Created joint: "<<name<<" id: "<<id<<endl;
        }

        void addChild(Joint* child) {
            children.push_back(child);
        }

        // void calculateInverseBindTranform(glm::mat4 parentBindTransform) {
        //     glm::mat4 bindTransform = parentBindTransform*localBindTransform;
        //     invBindTransform = glm::inverse(bindTransform);
        //     for(int i=0;i<children.size();i++) {
        //         children[i]->calculateInverseBindTranform(bindTransform);
        //     }
        // }

        void applyTransformation(map<string,glm::mat4>& update,glm::mat4 parentTranformation) {
            if(update.find(name) == update.end()) {
                cout<<"Error in apply transformation. Joint:"<<name<<" not found in update"<<endl;
                update[name] = glm::mat4(1.0f);
            }
            glm::mat4 localTransform = update[name];
            glm::mat4 currentTransform = parentTranformation*localTransform;
            animatedTransform = currentTransform*offsetTransform;
            for(int i=0;i<children.size();i++) {
                children[i]->applyTransformation(update,currentTransform);
            }
        }
};

struct JointTransform {
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;

    glm::mat4 getTransformation() {
        // return glm::translate(glm::toMat4(rotation)*glm::scale(glm::mat4(1.0f),scale),position);
        glm::mat4 trans = glm::translate(glm::mat4(1.0f),position);
        trans = trans*glm::toMat4(rotation);
        trans = glm::scale(trans,scale);
        return trans;
    }
};

class Animator {
    private:
        float duration;
        map<string,pair<vector<float>,vector<JointTransform>>> keyframes;
    public:
        Animator(aiAnimation* animation):duration{0} {
            float duration = animation->mTicksPerSecond*animation->mDuration;
            for(int i=0;i<animation->mNumChannels;i++) {
                aiNodeAnim* node = animation->mChannels[i];
                string jointName((node->mNodeName).C_Str());
                assert(node->mNumRotationKeys==node->mNumScalingKeys);
                assert(node->mNumRotationKeys==node->mNumPositionKeys);
                for(int j=0;j<(node->mNumRotationKeys);j++) {
                    float timestamp = (node->mRotationKeys[j]).mTime;
                    JointTransform transform;
                    transform.rotation = glm::quat(node->mRotationKeys[j].mValue.w,
                                                    node->mRotationKeys[j].mValue.x,
                                                    node->mRotationKeys[j].mValue.y,
                                                    node->mRotationKeys[j].mValue.z);
                    transform.position = glm::vec3(node->mPositionKeys[j].mValue.x,
                                                    node->mPositionKeys[j].mValue.y,
                                                    node->mPositionKeys[j].mValue.z);
                    transform.scale = glm::vec3(node->mScalingKeys[j].mValue.x,
                                                    node->mScalingKeys[j].mValue.y,
                                                    node->mScalingKeys[j].mValue.z);
                    keyframes[jointName].first.push_back(timestamp);
                    keyframes[jointName].second.push_back(transform);
                }
            }
        }

        JointTransform interpolate(JointTransform low, JointTransform high, float mix) {
            JointTransform result;
            result.position = low.position + (high.position-low.position)*mix;
            result.scale = low.scale + (high.scale-low.scale)*mix;
            result.rotation = glm::slerp(low.rotation,high.rotation,mix);
            return result;
        }

        glm::mat4 getTransformation(string jointName,float timestamp) {
            auto& jointKeyFrames = keyframes[jointName];
            if(timestamp <= jointKeyFrames.first[0]) {
                return jointKeyFrames.second[0].getTransformation();
            }
            if(timestamp >= jointKeyFrames.first.back()) {
                return jointKeyFrames.second.back().getTransformation();
            }
            auto itr = std::lower_bound(jointKeyFrames.first.begin(),jointKeyFrames.first.end(),timestamp);
            int highIdx = (itr-jointKeyFrames.first.begin());
            int lowIdx = highIdx--;
            float mix = (timestamp-jointKeyFrames.first[lowIdx])/(jointKeyFrames.first[highIdx]-jointKeyFrames.first[lowIdx]);

            auto interpKeyframe = interpolate(jointKeyFrames.second[lowIdx],jointKeyFrames.second[highIdx],mix);
            return interpKeyframe.getTransformation();
        }

        map<string,glm::mat4> getUpdate(float timestamp) {
            map<string,glm::mat4> update;
            for(auto const& key : keyframes) {
                update[key.first] = getTransformation(key.first,timestamp);
                // update[key.first] = glm::mat4(1.0f);
            }
            return update;
        }

};

class AnimatedModel {
    private:
        Joint* rootJoint;
        Animator* animator;
        map<string,int> jointName2Id;
        vector<Joint*> allJoints;
        vector<Texture> textures_loaded;
        vector<AnimatedMesh> meshes;
        string directory;
        glm::mat4 modelTransformation;


    public:
        AnimatedModel(const std::string& path):rootJoint{NULL},animator{NULL},modelTransformation{1.0f} {
            // read file via ASSIMP
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
            // check for errors
            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
            {
                cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
                return;
            }
            // retrieve the directory path of the filepath
            this->directory = path.substr(0, path.find_last_of('/'));
            parseModel(scene);
            
            modelTransformation = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,-40));
            modelTransformation = glm::rotate(modelTransformation,glm::radians(90.0f),glm::vec3(0,1,0));
            modelTransformation = glm::rotate(modelTransformation,glm::radians(-90.0f),glm::vec3(1,0,0));

        }

        void update(float timestamp) {
            map<string,glm::mat4> updateTranformation = animator->getUpdate(timestamp);
            rootJoint->applyTransformation(updateTranformation,modelTransformation);
        }

        // draws the model, and thus all its meshes
        void Draw(Shader shader)
        {
            // shader.setMat4("model",modelTransformation);
            for(int i=0;i<allJoints.size();i++) {
                shader.setMat4("jointTransforms["+std::to_string(i)+"]",allJoints[i]->animatedTransform);
            }
            glm::mat4 jointTransformMatrices[jointName2Id.size()];

            for(unsigned int i = 0; i < meshes.size(); i++)
                meshes[i].Draw(shader);
        }
        void parseModel(const aiScene* scene) {
            processMeshes(scene);
            parseJoints(scene->mRootNode,scene,NULL);
            parseAnimations(scene);
            // rootJoint->calculateInverseBindTranform(glm::mat4(1.0f));
        }

        void parseAnimations(const aiScene* scene) {
            assert(scene->mNumAnimations > 0);
            if(scene->mNumAnimations > 1) {
                cout<<"Number of animations is greater than 1. Taking only 1st animation"<<endl;
            }
            aiAnimation* animation = scene->mAnimations[0];
            animator = new Animator(animation);

        }

        void processMeshes(const aiScene* scene) {
            for(int i=0;i<scene->mNumMeshes;i++) {
                aiMesh* m = scene->mMeshes[i];
                meshes.push_back(processMesh(m,scene));
                
            }
        }

        void parseJoints(aiNode* node, const aiScene* scene, Joint* parentJoint) {
            Joint* nodeJoint = NULL;
            string name((node->mName).C_Str());
            if(jointName2Id.find(name)!=jointName2Id.end()) {
                int idx = jointName2Id[name];
                nodeJoint = allJoints[idx];
                nodeJoint->localBindTransform = aiMatrix4x4ToGlm(&(node->mTransformation));
                if(parentJoint != NULL) {
                    parentJoint->children.push_back(nodeJoint);
                }
            } else {
                nodeJoint = parentJoint;
            }

            if(rootJoint == NULL && nodeJoint != NULL) {
                rootJoint = nodeJoint;
            }

            
            // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
            for(unsigned int i = 0; i < node->mNumChildren; i++)
            {
                parseJoints(node->mChildren[i], scene, nodeJoint);
            }
        }

        AnimatedMesh processMesh(aiMesh *mesh, const aiScene *scene)
        {
            // data to fill
            vector<Vertex> vertices;
            vector<unsigned int> indices;
            vector<Texture> textures;
            map<int,vector<pair<float,int>>> vertexJointMap;
            
            // Walk through each of the mesh's bones
            for(unsigned int i = 0; i < mesh->mNumBones; i++) {
                aiBone* joint = mesh->mBones[i];
                string boneName((joint->mName).C_Str());
                if(jointName2Id.find(boneName)==jointName2Id.end()) {
                    int newIdx = jointName2Id.size();
                    assert(jointName2Id.size()==allJoints.size());
                    jointName2Id[boneName] = newIdx;
                    allJoints.push_back(new Joint(newIdx,boneName));
                    allJoints[newIdx]->offsetTransform = aiMatrix4x4ToGlm(&(joint->mOffsetMatrix));
                }
                int jointIdx = jointName2Id[(joint->mName).C_Str()];
                for(int j=0;j<joint->mNumWeights;j++) {
                    auto jointWeight = (joint->mWeights)[j];
                    vertexJointMap[jointWeight.mVertexId].push_back(make_pair(jointWeight.mWeight,jointIdx));
                }
            }

            // Walk through each of the mesh's vertices
            for(unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex;
                glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
                // positions
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.Position = vector;
                // normals
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
                // texture coordinates
                if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
                {
                    glm::vec2 vec;
                    // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                    // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                    vec.x = mesh->mTextureCoords[0][i].x; 
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.TexCoords = vec;
                }
                else
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);

                auto& joints = vertexJointMap[i];
                if(joints.size() == 0 ) {
                    // no joints for this vertex!!!
                    cout<<"No joint for vertex id: "<<i<<endl;
                }
                // initialize num joints
                vertex.NumJoints[0] = min((int)(joints.size()),3);

                // descending sort joints
                std::sort(joints.rbegin(), joints.rend()); 
                
                // joint indices && joint weights
                float totalWeight = 0.00001f;
                for(int j=0;j<3;j++) {
                    if(j<joints.size()) {
                        vertex.JointIndices[j] = joints[j].second;
                        vertex.JointWeights[j] = joints[j].first;
                        totalWeight += vertex.JointWeights[j];
                    } else {
                        vertex.JointIndices[j] = 0;
                        vertex.JointWeights[j] = 0;
                    }
                }
                // normalize weights
                for(int j=0;j<3;j++) {
                    vertex.JointWeights[j] /= totalWeight;
                }

                vertices.push_back(vertex);
            }
            // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
            for(unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                // retrieve all indices of the face and store them in the indices vector
                for(unsigned int j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }
            // process materials
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    

            // 1. diffuse maps
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
            
            // return a mesh object created from the extracted mesh data
            return AnimatedMesh(vertices, indices, textures);
        }


        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
        {
            vector<Texture> textures;
            for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);
                // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
                bool skip = false;
                for(unsigned int j = 0; j < textures_loaded.size(); j++)
                {
                    if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                    {
                        textures.push_back(textures_loaded[j]);
                        skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                        break;
                    }
                }
                if(!skip)
                {   // if texture hasn't been loaded already, load it
                    Texture texture;
                    texture.id = TextureFromFile(str.C_Str(), this->directory);
                    texture.type = typeName;
                    texture.path = str.C_Str();
                    textures.push_back(texture);
                    textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
                }
            }
            return textures;
        }

        inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
        {
            glm::mat4 to;

            to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
            to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
            to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
            to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

            return to;
        }
};

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

};
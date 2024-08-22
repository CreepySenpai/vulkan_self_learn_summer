#include <Creepy/Model.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// TODO: Remove Hard Code
const std::filesystem::path modelsDirectory{"./res/models"};

namespace Creepy{

    Model::Model(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue) {
        this->LoadModel(filePath, device, commandPool, queue);
    }
            
    void Model::Destroy(const vk::Device device) const {
        for(auto&& mesh : m_meshes){
            mesh.Destroy(device);
        }
    }

    std::span<const Mesh> Model::GetMeshes() const {
        return m_meshes;
    }

    std::span<Mesh> Model::GetMeshes() {
        return m_meshes;
    }

    void Model::LoadModel(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue){
        Assimp::Importer importer{};

        auto scene = importer.ReadFile(filePath.string(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

        if(!scene){
            std::println("Failed to load model: {}", filePath.string());
        }
        
        this->processNode(scene->mRootNode, scene, device, commandPool, queue);

        std::println("Total Mesh: {}", m_meshes.size());
    }

    void Model::processNode(aiNode* currentNode, const aiScene* currentScene, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue){
        
        // Check Mesh Current Node
        for(uint32_t i{}; i < currentNode->mNumMeshes; ++i){
            auto currentMesh = currentScene->mMeshes[currentNode->mMeshes[i]];
            m_meshes.push_back(this->processMesh(currentMesh, currentScene, device, commandPool, queue));
        }

        // Check Child Node
        for(uint32_t i{}; i < currentNode->mNumChildren; ++i){
            processNode(currentNode->mChildren[i], currentScene, device, commandPool, queue);
        }
    }

    Mesh Model::processMesh(aiMesh* currentMesh, const aiScene* currentScene, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue){
        std::vector<Vertex> vertices(currentMesh->mNumVertices);

        for(uint32_t i{}; i < currentMesh->mNumVertices; ++i){
            vertices[i].Position.x = currentMesh->mVertices[i].x;
            vertices[i].Position.y = currentMesh->mVertices[i].y;
            vertices[i].Position.z = currentMesh->mVertices[i].z;


            vertices[i].Normal.x = currentMesh->mNormals[i].x;
            vertices[i].Normal.y = currentMesh->mNormals[i].y;
            vertices[i].Normal.z = currentMesh->mNormals[i].z;
            
            
            if(currentMesh->mTextureCoords[0]){
                vertices[i].TexCoord.x = currentMesh->mTextureCoords[0][i].x;
                vertices[i].TexCoord.y = currentMesh->mTextureCoords[0][i].y;
            }
        }

        std::vector<uint32_t> indices;

        for(uint32_t i{}; i < currentMesh->mNumFaces; ++i){
            
            auto currentFace = currentMesh->mFaces[i];

            for(uint32_t j{}; j < currentFace.mNumIndices; ++j){
                indices.emplace_back(currentFace.mIndices[j]);
            }

        }

        auto&& textures = this->loadMaterialTextures(currentScene->mMaterials[currentMesh->mMaterialIndex], currentScene, device, commandPool, queue);

        return {device, commandPool, queue, vertices, indices, textures};
    }

    std::vector<Texture> Model::loadMaterialTextures(aiMaterial* currentMaterial, const aiScene* currentScene, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue) {
        std::vector<Texture> textures;
        const uint32_t diffuseTextureCount{currentMaterial->GetTextureCount(aiTextureType_DIFFUSE)};
        
        std::println("Total diffuse texture: {}", diffuseTextureCount);

        for(uint32_t i{}; i < diffuseTextureCount; ++i){
            aiString filePath{};
            auto res = currentMaterial->GetTexture(aiTextureType_DIFFUSE, i, &filePath);
            
            if(res == aiReturn_SUCCESS){
                std::println("Get Texture: {}", filePath.C_Str());
                Texture texture{};
                texture.LoadTexture(modelsDirectory / filePath.C_Str(), device, commandPool, queue);
                textures.push_back(std::move(texture));
            }
        }

        return textures;
    }

}